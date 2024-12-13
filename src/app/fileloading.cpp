#include "FileLoading.h"


FileLoading::FileLoading(QObject *parent) : QObject(parent) {}

void FileLoading::loadFiles(const QStringList &fileUrls) {
    qDebug() << "Loading files:" << fileUrls;

    for (const QString &urlString : fileUrls) {
        QUrl fileUrl(urlString);
        if (!fileUrl.isValid()) {
            qWarning() << "Invalid file URL:" << fileUrl;
            continue;
        }

        QString localPath = fileUrl.toLocalFile();
        if (localPath.isEmpty()) {
            qWarning() << "Empty file path for URL:" << fileUrl;
            continue;
        }

        qDebug() << "Processing file:" << localPath;

        // Determine file extension
        QFileInfo fileInfo(localPath);
        QString fileExtension = fileInfo.suffix().toLower();

        vtkSmartPointer<vtkImageData> imageData;

        // Use appropriate reader based on file extension
        if (fileExtension == "jpg" || fileExtension == "jpeg") {
            imageData = loadJPEG(localPath);
        } else if (fileExtension == "png") {
            imageData = loadPNG(localPath);
        } else if (fileExtension == "dcm") {
            imageData = loadDICOM(localPath);
        } else if (fileExtension == "nii") {
            imageData = loadNIFTI(localPath);
        } else if (fileExtension == "gz" && localPath.endsWith(".nii.gz")) {
            QString decompressedPath = localPath.left(localPath.size() - 3); // Remove '.gz'
            if (decompressGZ(localPath.toStdString(), decompressedPath.toStdString())) {
                imageData = loadNIFTI(decompressedPath);
            } else {
                qWarning() << "Failed to decompress file:" << localPath;
                continue;
            }
        } else if (fileExtension == "vtk") {
            imageData = loadVTK(localPath);
        } else {
            qWarning() << "Unsupported file format:" << fileExtension;
            continue;
        }

        // Check if the image data was successfully loaded
        if (imageData) {
            qDebug() << "Successfully loaded image:" << localPath;
            visualizeSlicesWithSlider(imageData); // Call visualization method
        } else {
            qWarning() << "Failed to load image:" << localPath;
        }
    }
}

vtkSmartPointer<vtkImageData> FileLoading::loadJPEG(const QString &filePath) {
    qDebug() << "Loading JPEG file:" << filePath;
    auto reader = vtkSmartPointer<vtkJPEGReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    reader->Update();
    return reader->GetOutput();
}

vtkSmartPointer<vtkImageData> FileLoading::loadPNG(const QString &filePath) {
    qDebug() << "Loading PNG file:" << filePath;
    auto reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    reader->Update();
    return reader->GetOutput();
}

vtkSmartPointer<vtkImageData> FileLoading::loadDICOM(const QString &filePath) {
    qDebug() << "Loading DICOM file:" << filePath;
    auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    reader->Update();
    return reader->GetOutput();
}

vtkSmartPointer<vtkImageData> FileLoading::loadNIFTI(const QString &filePath) {
    qDebug() << "Loading NIfTI file:" << filePath;
    auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    reader->Update();
    return reader->GetOutput();
}

vtkSmartPointer<vtkImageData> FileLoading::loadVTK(const QString &filePath) {
    qDebug() << "Loading VTK file:" << filePath;
    auto reader = vtkSmartPointer<vtkDataSetReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    reader->Update();

    auto imageData = vtkImageData::SafeDownCast(reader->GetOutput());
    if (!imageData) {
        qWarning() << "The VTK file does not contain vtkImageData.";
        return nullptr;
    }

    return imageData;
}

void FileLoading::visualizeSlicesWithSlider(vtkSmartPointer<vtkImageData> imageData) {
    if (!imageData) {
        qWarning() << "No valid image data to visualize.";
        return;
    }

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

    // Create render window and interactor
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

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

    // Callback to update the slice based on slider value
    auto sliderCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    sliderCallback->SetCallback([](vtkObject* caller, long unsigned int, void* clientData, void*) {
        auto sliderWidget = static_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
        auto sliceMapper = static_cast<vtkImageSliceMapper*>(clientData);
        sliceMapper->SetSliceNumber(static_cast<int>(value));
    });
    sliderWidget->AddObserver(vtkCommand::InteractionEvent, sliderCallback);
    sliderCallback->SetClientData(sliceMapper);

    // Start rendering
    renderWindow->Render();
    renderWindowInteractor->Start();
}

bool FileLoading::decompressGZ(const std::string& gzFilePath, const std::string& outputFilePath) {
    gzFile gzFile = gzopen(gzFilePath.c_str(), "rb");
    if (!gzFile) return false;

    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        gzclose(gzFile);
        return false;
    }

    char buffer[4096];
    int bytesRead;
    while ((bytesRead = gzread(gzFile, buffer, sizeof(buffer))) > 0) {
        outputFile.write(buffer, bytesRead);
    }

    gzclose(gzFile);
    outputFile.close();

    return true;
}
