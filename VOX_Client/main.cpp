#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Core/connection.h"
#include "Core/UIManager/uimanager.h"
#include "Core/VoiceEngine/voiceengine.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QVoiceEngine& VoiceEngine = QVoiceEngine::getInstance();
    engine.rootContext()->setContextProperty("VoiceEngine", &VoiceEngine);

    QConnection& Connection = QConnection::getInstance();
    engine.rootContext()->setContextProperty("ClientConnection", &Connection);


    QUIManager& UIManager = QUIManager::getInstance();
    engine.rootContext()->setContextProperty("UIStateManager", &UIManager);
    engine.rootContext()->setContextProperty("ServerManager", UIManager.GetServerManager());
    engine.rootContext()->setContextProperty("FriendsManager", UIManager.GetFriendManager());
    engine.rootContext()->setContextProperty("ChatManager", UIManager.GetChatManager());
    engine.rootContext()->setContextProperty("UserManager", UIManager.GetUserManager());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("VOX_Client", "Main");


    return QGuiApplication::exec();
}
