#include "vtkwindow.h"
#include <vtkNIFTIImageReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkStructuredPoints.h>
#include <vtkImageThreshold.h>
#include <vtkImageHistogramStatistics.h>
#include <QDebug>
#include <vtkImageSobel3D.h>
#include <vtkImageCast.h>
#include <vtkImageMask.h>
#include <vtkImageData.h>
#include <vtkImageStencil.h>
#include <vtkImageToImageStencil.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageDilateErode3D.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkMassProperties.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>

VTKWindow::VTKWindow(QObject *parent) : QObject(parent) {
    // Initialize VTK components
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback([](vtkObject* caller, unsigned long, void* clientData, void*) {
        auto* self = static_cast<VTKWindow*>(clientData);
        self->updateZoomLevel();
    });

    renderWindowInteractor->AddObserver(vtkCommand::StartEvent, callback);
    callback->SetClientData(this);

}

VTKWindow::~VTKWindow() {}

void VTKWindow::showWindow() {
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void VTKWindow::loadFile2(const QString &filePath) {
    vtkSmartPointer<vtkImageData> imageData;

    if (filePath.endsWith(".nii") || filePath.endsWith(".nii.gz")) {
        auto niftiReader = vtkSmartPointer<vtkNIFTIImageReader>::New();
        niftiReader->SetFileName(filePath.toStdString().c_str());
        niftiReader->Update();
        imageData = niftiReader->GetOutput();
    } else if (filePath.endsWith(".vtk")) {
        auto vtkReader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        vtkReader->SetFileName(filePath.toStdString().c_str());
        vtkReader->Update();
        imageData = vtkSmartPointer<vtkImageData>::Take(vtkImageData::SafeDownCast(vtkReader->GetOutput()));
    } else {
        qWarning() << "Unsupported file format: " << filePath;
        return;
    }

    if (!imageData) {
        qWarning() << "Failed to load image data. Check the file format and contents.";
        return;
    }

    auto volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetInputData(imageData);

    auto colorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorFunction->AddRGBPoint(-1000, 0.0, 0.0, 0.0);
    colorFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);

    auto opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(-1000, 0.0);
    opacityFunction->AddPoint(0, 1.0);

    auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorFunction);
    volumeProperty->SetScalarOpacity(opacityFunction);
    volumeProperty->ShadeOn();

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    renderer->AddVolume(volume);
    renderer->ResetCamera();
    renderWindow->Render();

    double lowerThreshold = 50;
    double upperThreshold = 800;

    auto threshold = vtkSmartPointer<vtkImageThreshold>::New();
    threshold->SetInputData(imageData);
    threshold->ThresholdBetween(lowerThreshold, upperThreshold);
    threshold->ReplaceInOn();
    threshold->SetInValue(255); // Set white regions to 255
    threshold->ReplaceOutOn();
    threshold->SetOutValue(0);  // Set everything else to 0 (black)
    threshold->Update();

    auto segmentedData = threshold->GetOutput();

    auto sobelFilter = vtkSmartPointer<vtkImageSobel3D>::New();
    sobelFilter->SetInputData(segmentedData);
    sobelFilter->Update();

    auto edgeImage = sobelFilter->GetOutput();

    auto castFilter = vtkSmartPointer<vtkImageCast>::New();
    castFilter->SetInputData(edgeImage);
    castFilter->SetOutputScalarTypeToUnsignedChar();
    castFilter->Update();

    auto imageStencil = vtkSmartPointer<vtkImageToImageStencil>::New();
    imageStencil->SetInputData(castFilter->GetOutput());
    imageStencil->ThresholdByUpper(255);
    imageStencil->Update();

    auto imageMask = vtkSmartPointer<vtkImageStencil>::New();
    imageMask->SetInputData(segmentedData);
    imageMask->SetStencilData(imageStencil->GetOutput());
    imageMask->ReverseStencilOff();
    imageMask->SetBackgroundValue(0);
    imageMask->Update();

    // Masking Step: Already performed in your code
    auto maskedData = imageMask->GetOutput();

    // Step 1: Fill Gaps Using Morphological Closing
    auto dilateErode = vtkSmartPointer<vtkImageDilateErode3D>::New();
    dilateErode->SetInputData(maskedData);
    dilateErode->SetDilateValue(1);  // Expand white regions (fill gaps)
    dilateErode->SetErodeValue(0);   // Shrink black regions
    dilateErode->SetKernelSize(5, 5, 5);  // Larger kernel for more filling
    dilateErode->Update();

    // Step 2: Smooth the Result to Remove Noise
    auto gaussianSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gaussianSmooth->SetInputData(dilateErode->GetOutput());
    gaussianSmooth->SetStandardDeviations(1.0, 1.0, 1.0);  // Smoothing strength
    gaussianSmooth->SetRadiusFactors(2.0, 2.0, 2.0);       // Radius of smoothing
    gaussianSmooth->Update();


    // Step 3: Extract Solid Surface Using Marching Cubes
    auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
    marchingCubes->SetInputData(gaussianSmooth->GetOutput());
    marchingCubes->SetValue(0, 1.0);  // Iso-value for the surface extraction
    marchingCubes->Update();

    // Step 4: Map the Extracted Surface to PolyData
    auto surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    surfaceMapper->SetInputConnection(marchingCubes->GetOutputPort());
    surfaceMapper->ScalarVisibilityOff();  // Hide scalar colors, ensure solid color

    // Step 5: Surface Actor Configuration
    auto surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(surfaceMapper);
    surfaceActor->GetProperty()->SetColor(1.0, 1.0, 1.0);  // Solid white surface
    surfaceActor->GetProperty()->SetOpacity(1.0);          // Fully opaque

    // Step 6: Set Up a New Renderer for Output
    auto solidSurfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
    solidSurfaceRenderer->SetBackground(0.0, 0.0, 0.0);  // Black background

    auto solidSurfaceRenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    solidSurfaceRenderWindow->AddRenderer(solidSurfaceRenderer);

    auto solidSurfaceInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    solidSurfaceInteractor->SetRenderWindow(solidSurfaceRenderWindow);

    // Step 7: Add the Solid Surface Actor to the Renderer
    solidSurfaceRenderer->AddActor(surfaceActor);
    solidSurfaceRenderer->ResetCamera();

    // Step 8: Render the Final Output
    solidSurfaceRenderWindow->Render();
    qDebug() << "Solid surface visualization complete.";
    solidSurfaceInteractor->Start();
}



void VTKWindow::loadFile(const QString &filePath) {
    this->filePath = filePath;
    processImage();
}

void VTKWindow::updateSettings(double lowerThreshold, double upperThreshold, int kernelSize, double smoothingStdDev) {
    this->lowerThreshold = lowerThreshold;
    this->upperThreshold = upperThreshold;
    this->kernelSize = kernelSize;
    this->smoothingStdDev = smoothingStdDev;
    processImage();
}

void VTKWindow::processImage() {
    vtkSmartPointer<vtkImageData> imageData;

    if (filePath.endsWith(".nii") || filePath.endsWith(".nii.gz")) {
        auto niftiReader = vtkSmartPointer<vtkNIFTIImageReader>::New();
        niftiReader->SetFileName(filePath.toStdString().c_str());
        niftiReader->Update();
        imageData = niftiReader->GetOutput();
    } else if (filePath.endsWith(".vtk")) {
        auto vtkReader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        vtkReader->SetFileName(filePath.toStdString().c_str());
        vtkReader->Update();
        imageData = vtkSmartPointer<vtkImageData>::Take(vtkImageData::SafeDownCast(vtkReader->GetOutput()));
    } else {
        qWarning() << "Unsupported file format: " << filePath;
        return;
    }

    if (!imageData) {
        qWarning() << "Failed to load image data. Check the file format and contents.";
        return;
    }

    /// Get Image Dimensions
    // Get image dimensions
    int dims[3];
    imageData->GetDimensions(dims);  // dims[0] = width, dims[1] = height, dims[2] = depth

    m_width = QString::number(dims[0]);
    m_height = QString::number(dims[1]);
    m_depth = QString::number(dims[2]);

    // Volume Rendering Setup
    auto volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetInputData(imageData);

    auto colorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorFunction->AddRGBPoint(-1000, 0.0, 0.0, 0.0);
    colorFunction->AddRGBPoint(0, 1.0, 1.0, 1.0);

    auto opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(-1000, 0.0);
    opacityFunction->AddPoint(0, 1.0);

    auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorFunction);
    volumeProperty->SetScalarOpacity(opacityFunction);
    volumeProperty->ShadeOn();

    auto volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // Surface Extraction Workflow
    auto threshold = vtkSmartPointer<vtkImageThreshold>::New();
    threshold->SetInputData(imageData);
    threshold->ThresholdBetween(lowerThreshold, upperThreshold);
    threshold->ReplaceInOn();
    threshold->SetInValue(255);
    threshold->ReplaceOutOn();
    threshold->SetOutValue(0);
    threshold->Update();

    auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
    marchingCubes->SetInputData(threshold->GetOutput());
    marchingCubes->SetValue(0, 1.0);
    marchingCubes->Update();

    auto surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    surfaceMapper->SetInputConnection(marchingCubes->GetOutputPort());
    surfaceMapper->ScalarVisibilityOff();

    auto surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(surfaceMapper);
    surfaceActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    surfaceActor->GetProperty()->SetOpacity(1.0);

    // Create Two Separate Renderers
    auto volumeRenderer = vtkSmartPointer<vtkRenderer>::New();
    auto surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();

    // Setup Viewports
    volumeRenderer->SetViewport(0.0, 0.0, 0.5, 1.0);  // Left
    surfaceRenderer->SetViewport(0.5, 0.0, 1.0, 1.0); // Right

    // Add Actors
    volumeRenderer->AddVolume(volume);
    surfaceRenderer->AddActor(surfaceActor);

    // Clear Previous Renderers
    if (renderWindow->GetRenderers()->GetNumberOfItems() > 0) {
        auto renderers = renderWindow->GetRenderers();
        renderers->InitTraversal();
        vtkRenderer *renderer;
        while ((renderer = renderers->GetNextItem()) != nullptr) {
            renderWindow->RemoveRenderer(renderer);
        }
    }

    /// Calculate Volume
    auto massProperties = vtkSmartPointer<vtkMassProperties>::New();
    massProperties->SetInputConnection(marchingCubes->GetOutputPort());
    massProperties->Update();

    double calculatedVolume = massProperties->GetVolume();
    m_volume = QString::number(calculatedVolume, 'f', 2);  // Store volume as a string

    // Calculate Surface Area
    auto massPropertiesR = vtkSmartPointer<vtkMassProperties>::New();
    massPropertiesR->SetInputConnection(marchingCubes->GetOutputPort());
    massPropertiesR->Update();

    double surfaceArea = massPropertiesR->GetSurfaceArea();
    qDebug() << "Surface Area:" << surfaceArea;
    m_surfaceArea = QString::number(surfaceArea, 'f', 2);  // Store as a string
    emit surfaceAreaChanged();


    emit volumeChanged();
    m_modelLoaded = true;
    emit dimensionsChanged();
    emit modelLoadedChanged();


    // Add New Renderers
    renderWindow->AddRenderer(volumeRenderer);
    renderWindow->AddRenderer(surfaceRenderer);

    // Reset Cameras and Render
    volumeRenderer->ResetCamera();
    surfaceRenderer->ResetCamera();

    renderWindow->Render();
    renderWindowInteractor->Start();
}

void VTKWindow::updateZoomLevel() {
    auto camera = renderer->GetActiveCamera();
    m_zoomLevel = camera->GetDistance();  // Perspective mode
    emit zoomLevelChanged();
}


