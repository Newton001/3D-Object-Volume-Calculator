#ifndef FILELOADING_H
#define FILELOADING_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QUrl>
#include <QFileInfo>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkDICOMImageReader.h>
#include <vtkNIFTIImageReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPointPicker.h>
#include <vtkCommand.h>
#include <vtkRendererCollection.h>
#include <vtkDataSetReader.h>
#include <zlib.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkCallbackCommand.h>

class FileLoading : public QObject {
    Q_OBJECT
public:
    explicit FileLoading(QObject *parent = nullptr);

    Q_INVOKABLE void loadFiles(const QStringList &fileUrls);

private:
    vtkSmartPointer<vtkImageData> loadJPEG(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadPNG(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadDICOM(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadNIFTI(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadVTK(const QString &filePath);
    bool decompressGZ(const std::string& gzFilePath, const std::string& outputFilePath);

    void visualizeSlicesWithSlider(vtkSmartPointer<vtkImageData> imageData);
};

#endif // FILELOADING_H
