#ifndef VTKWINDOW_H
#define VTKWINDOW_H

#include <QObject>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>

class VTKWindow : public QObject {
    Q_OBJECT
public:
    explicit VTKWindow(QObject *parent = nullptr);
    ~VTKWindow();

    Q_INVOKABLE void showWindow();
    Q_INVOKABLE void loadFile(const QString &filePath); // Load a single file

private:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkVolume> volume;
};

#endif // VTKWINDOW_H
