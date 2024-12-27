#include <QApplication> // Replace QGuiApplication with QApplication
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app/fileloading.h"   // File loader for managing file inputs
#include "app/vtkwindow.h"     // VTK window for rendering and computation

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv); // Use QApplication instead of QGuiApplication

    // Set organization details (helps with QSettings warnings)
    app.setOrganizationName("MyOrganization");
    app.setOrganizationDomain("myorganization.com");

    QQmlApplicationEngine engine;

    // Create an instance of FileLoading and expose it to QML
    FileLoading fileLoader;
    engine.rootContext()->setContextProperty("fileLoader", &fileLoader);

    // Create an instance of VTKWindow and expose it to QML
    VTKWindow* vtkWindow = VTKWindow::getInstance();
    engine.rootContext()->setContextProperty("vtkWindow", vtkWindow);

    // Load the main QML file
    const QUrl url(QStringLiteral("qrc:/welcome.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
