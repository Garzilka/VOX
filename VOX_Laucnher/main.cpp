#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "launchercontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QLauncherController& Connection = QLauncherController::getInstance();
    engine.rootContext()->setContextProperty("LauncherCtrl", &Connection);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("NomadUpdater", "Main");


    return QGuiApplication::exec();
}
