#include "FileLoading.h"
#include <thread>
#include "vtkwindow.h"
#include <vector>


FileLoading::FileLoading(QObject *parent) {}

// Global vector to store voxel counts
std::vector<size_t> voxelCounts;

// Helper function to emit voxel data signal
void emitVoxelData(VTKWindow* vtkWindow) {
    QVector<int> slices, counts;
    for (size_t i = 0; i < voxelCounts.size(); ++i) {
        slices.append(static_cast<size_t>(i));
        counts.append(voxelCounts[i]);
    }
    emit vtkWindow->voxelDataUpdated(slices, counts);
}


void FileLoading::visualizeSlicesWithSlider() {
    qWarning() << "Starting ...";
    VTKWindow *vtkWindow = VTKWindow::getInstance();
    vtkSmartPointer<vtkImageData> imageData = vtkWindow->getImageData();

    if (!imageData) {
        qWarning() << "No valid image data to visualize.";
        return;
    } else {
        qWarning() << "Valid image data to visualize.";
    }

    // Start a new thread for the 2D slice viewer
    std::thread([imageData, vtkWindow]() {
        // Create a slice mapper
        auto sliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
        sliceMapper->SetInputData(imageData);
        sliceMapper->SetSliceNumber(0); // Start with the first slice
        sliceMapper->SetOrientationToZ(); // Slicing along Z-axis

        auto slice = vtkSmartPointer<vtkImageSlice>::New();
        slice->SetMapper(sliceMapper);

        // Create a renderer and add the slice
        auto renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->AddViewProp(slice);
        renderer->ResetCamera();

        // Create a new render window for this visualization
        auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->SetWindowName("2D Slice Viewer");
        renderWindow->AddRenderer(renderer);

        // Create render window interactor and set up interactor style
        auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        auto interactorStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
        renderWindowInteractor->SetInteractorStyle(interactorStyle);
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Create a slider representation
        auto sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
        sliderRep->SetMinimumValue(0); // First slice
        sliderRep->SetMaximumValue(imageData->GetDimensions()[2] - 1); // Last slice
        sliderRep->SetValue(0); // Initial slice
        sliderRep->SetTitleText("Slice");

        // Position the slider in the render window
        sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.1); // Bottom-left
        sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
        sliderRep->GetPoint2Coordinate()->SetValue(0.3, 0.1); // Bottom-right

        // Create the slider widget
        auto sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
        sliderWidget->SetInteractor(renderWindowInteractor);
        sliderWidget->SetRepresentation(sliderRep);
        sliderWidget->SetAnimationModeToAnimate();
        sliderWidget->EnabledOn();

        // Precompute voxel counts for all slices
        int dims[3];
        imageData->GetDimensions(dims);
        voxelCounts.resize(dims[2], 0);

        for (int z = 0; z < dims[2]; ++z) {
            size_t voxelCount = 0;
            for (size_t y = 0; y < dims[1]; ++y) {
                for (size_t x = 0; x < dims[0]; ++x) {
                    double* voxelValue = static_cast<double*>(imageData->GetScalarPointer(x, y, z));
                    if (voxelValue && *voxelValue > 50.0) { // Example threshold
                        ++voxelCount;
                    }
                }
            }
            voxelCounts[z] = voxelCount;
        }

        // Emit initial data for the entire volume
        emitVoxelData(vtkWindow);

        // Callback to update the slice number
        auto sliderCallback = vtkSmartPointer<vtkCallbackCommand>::New();
        sliderCallback->SetCallback([](vtkObject* caller, long unsigned int, void* clientData, void*) {
            auto sliderWidget = static_cast<vtkSliderWidget*>(caller);
            double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
            auto sliceMapper = static_cast<vtkImageSliceMapper*>(clientData);
            sliceMapper->SetSliceNumber(static_cast<int>(value));

            int sliceIndex = static_cast<int>(value);
            qDebug() << "Slice" << sliceIndex << ": Voxel Count =" << voxelCounts[sliceIndex];
        });
        sliderWidget->AddObserver(vtkCommand::InteractionEvent, sliderCallback);
        sliderCallback->SetClientData(sliceMapper);

        // Show the window and start interaction
        renderWindow->Render();
        renderWindowInteractor->Start();
    }).detach(); // Detach the thread to run independently
}

