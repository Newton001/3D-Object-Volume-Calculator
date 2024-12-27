#ifndef FILELOADING_H
#define FILELOADING_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkNIFTIImageReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkImageThreshold.h>
#include <vtkImageSobel3D.h>
#include <vtkImageCast.h>
#include <vtkImageToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageDilateErode3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <zlib.h>
#include <vtkInteractorStyleImage.h>
#include <vtkProperty.h>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include "vtkwindow.h"



class FileLoading : public QObject {
    Q_OBJECT

public:
    explicit FileLoading(QObject *parent = nullptr);

    // Visualizes slices of the image data using a slider
    Q_INVOKABLE void visualizeSlicesWithSlider();

    // Decompresses a .gz file to a specified output path
    bool decompressGZ(const std::string &gzFilePath, const std::string &outputFilePath);

signals:
    void voxelDataUpdated(QVector<int> sliceIndices, QVector<int> voxelCounts);


private:
    VTKWindow *vtkWindow; // Instance to access getImageData
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkVolume> volume;
};

#endif // FILELOADING_H
