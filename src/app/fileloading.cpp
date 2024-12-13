// File: FileLoading.cpp
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
        } else {
            qWarning() << "Unsupported file format:" << fileExtension;
            continue;
        }

        // Check if the image data was successfully loaded
        if (imageData) {
            qDebug() << "Successfully loaded image:" << localPath;
            visualizeImage(imageData); // Call visualization method
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

void FileLoading::visualizeImage(vtkSmartPointer<vtkImageData> imageData) {
    if (!imageData) {
        qWarning() << "No valid image data to visualize.";
        return;
    }

    // Create a mapper and actor for the image
    auto mapper = vtkSmartPointer<vtkImageMapper>::New();
    mapper->SetInputData(imageData);
    mapper->SetColorWindow(255);
    mapper->SetColorLevel(127.5);

    auto actor = vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);

    // Create a renderer, render window, and interactor
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.1, 0.2, 0.3);

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Start the rendering process
    renderWindow->Render();
    renderWindowInteractor->Start();
}

