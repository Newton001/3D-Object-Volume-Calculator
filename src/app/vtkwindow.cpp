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
#include <QDebug>

VTKWindow::VTKWindow(QObject *parent) : QObject(parent) {
    // Initialize VTK components
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);
}

VTKWindow::~VTKWindow() {}

void VTKWindow::showWindow() {
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void VTKWindow::loadFile(const QString &filePath) {
    vtkSmartPointer<vtkImageData> imageData;

    if (filePath.endsWith(".nii") || filePath.endsWith(".nii.gz")) {
        // Use NIFTI reader for .nii or .nii.gz files
        auto niftiReader = vtkSmartPointer<vtkNIFTIImageReader>::New();
        niftiReader->SetFileName(filePath.toStdString().c_str());
        niftiReader->Update();
        imageData = niftiReader->GetOutput(); // Already vtkImageData
    } else if (filePath.endsWith(".vtk")) {
        // Use structured points reader for .vtk files
        auto vtkReader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        vtkReader->SetFileName(filePath.toStdString().c_str());
        vtkReader->Update();

        // Safely cast vtkStructuredPoints to vtkImageData
        auto structuredPoints = vtkReader->GetOutput();
        imageData = vtkSmartPointer<vtkImageData>::Take(vtkImageData::SafeDownCast(structuredPoints));
    } else {
        qWarning() << "Unsupported file format: " << filePath;
        return;
    }

    if (!imageData) {
        qWarning() << "Failed to load image data. Check the file format and contents.";
        return;
    }

    // Set up the volume mapper
    auto volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetInputData(imageData);

    // Set up the volume property
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

    // Create the volume and add it to the renderer
    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    renderer->AddVolume(volume);
    renderer->ResetCamera();
    renderWindow->Render();
}
