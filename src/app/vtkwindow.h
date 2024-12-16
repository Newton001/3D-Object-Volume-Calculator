#ifndef VTKWINDOW_H
#define VTKWINDOW_H

#include <QObject>
#include <QString>
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

    // Methods exposed to QML
    Q_INVOKABLE void showWindow();
    Q_INVOKABLE void loadFile(const QString &filePath);
    Q_INVOKABLE void loadFile2(const QString &filePath);
    Q_INVOKABLE void updateSettings(double lowerThreshold, double upperThreshold, int kernelSize, double smoothingStdDev);

    // Properties exposed to QML
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString width READ width NOTIFY dimensionsChanged)
    Q_PROPERTY(QString height READ height NOTIFY dimensionsChanged)
    Q_PROPERTY(QString depth READ depth NOTIFY dimensionsChanged)
    Q_PROPERTY(QString volumer READ volume NOTIFY volumeChanged)
    Q_PROPERTY(bool modelLoaded READ modelLoaded NOTIFY modelLoadedChanged)
    Q_PROPERTY(QString surfaceArea READ surfaceArea NOTIFY dimensionsChanged)

    // Property getters
    QString fileName() const { return m_fileName; }
    QString width() const { return m_width; }
    QString height() const { return m_height; }
    QString depth() const { return m_depth; }
    QString volumer() const { return m_volume; }
    QString surfaceArea() const { return m_surfaceArea; }
    double zoomLevel() const { return m_zoomLevel; }
    QString setRenderingMode(const QString &mode);
    bool modelLoaded() const { return m_modelLoaded; }

signals:
    void fileNameChanged();
    void dimensionsChanged();
    void volumeChanged();
    void modelLoadedChanged();
    void renderingModeChanged();
    void surfaceAreaChanged();
    void zoomLevelChanged();

private:
    // Member variables
    QString filePath;
    QString m_fileName;
    QString m_width, m_height, m_depth, m_volume, m_surfaceArea;
    double m_zoomLevel = 0.0;
    QString m_renderingMode;
    bool m_modelLoaded = false;

    void updateZoomLevel();

    double lowerThreshold = 50.0;
    double upperThreshold = 800.0;
    int kernelSize = 5;
    double smoothingStdDev = 1.0;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> surfaceActor;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkVolume> volume;

    void processImage();
};

#endif // VTKWINDOW_H
