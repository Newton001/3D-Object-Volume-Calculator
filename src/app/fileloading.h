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
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>


class FileLoading : public QObject {
    Q_OBJECT
public:
    explicit FileLoading(QObject *parent = nullptr);

    Q_INVOKABLE void loadFiles(const QStringList &fileUrls);

private:
    vtkSmartPointer<vtkImageData> loadJPEG(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadPNG(const QString &filePath);
    vtkSmartPointer<vtkImageData> loadDICOM(const QString &filePath);

    void visualizeImage(vtkSmartPointer<vtkImageData> imageData);
};

#endif // FILELOADING_H
