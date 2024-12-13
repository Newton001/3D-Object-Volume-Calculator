#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app/fileloading.h" // Include the FileLoading header

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    // Set organization details (helps with QSettings warnings)
    app.setOrganizationName("MyOrganization");
    app.setOrganizationDomain("myorganization.com");

    QQmlApplicationEngine engine;

    // Create an instance of FileLoading and expose it to QML
    FileLoading fileLoader;
    engine.rootContext()->setContextProperty("mainWindow", &fileLoader);

    // Load the main QML file
    const QUrl url(QStringLiteral("qrc:/main.qml"));
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
