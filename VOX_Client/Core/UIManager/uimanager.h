#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include "Core/UIManager/Server/servermanager.h"
#include "Core/UIManager/Friend/friendmanager.h"
#include "Core/UIManager/ChatManager/chatmanager.h"
#include "Core/UIManager/UsersManager/usersmanager.h"

class QUIManager  : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isCreateChannelDialogOpen READ isCreateChannelDialogOpen WRITE setCreateChannelDialogOpen NOTIFY isCreateChannelDialogOpenChanged)
    Q_PROPERTY(int defaultChannelType READ getDefaultChannelType WRITE setDefaultChannelType NOTIFY defaultChannelTypeChanged)
    Q_PROPERTY(bool isCreateServerDialogOpen READ isCreateServerDialogOpen WRITE setCreateServerDialogOpen NOTIFY isCreateServerDialogOpenChanged)
    Q_PROPERTY(bool securitySuccessfully READ getSecuritySuccessfully WRITE setSecuritySuccessfully NOTIFY securitySuccessfullyChanged)

    // ТВОЯ НАСТРОЙКА СВОЙСТВ:
    Q_PROPERTY(int MainUIState READ getMainUIState WRITE setMainUIState NOTIFY mainUIStateChanged) // 0 - Auth | 1 - WorkSpace | 2 - Settings
    Q_PROPERTY(int WorkState READ getWorkState WRITE setWorkState NOTIFY workStateChanged)         // 0 - Друзья | 1 - Сервер

private:
    int MainUIState = 0;
    int WorkState = 0;
    bool SecuritySuccessfully = false;

    bool m_isCreateChannelDialogOpen = false;
    int m_defaultChannelType = 2; // 2 - Текст, 5 - Голос
    bool m_isCreateServerDialogOpen = false;

public:
    QUIManager& operator=(const QUIManager&) = delete;
    QUIManager(const QUIManager&) = delete;
    static QUIManager& getInstance()
    {
        static QUIManager instance(nullptr);
        return instance;
    };

    bool getSecuritySuccessfully() { return SecuritySuccessfully; }
    int getMainUIState() { return MainUIState; }
    int getWorkState() { return WorkState; }

    Q_INVOKABLE void setMainUIState(int NewNum)
    {
        if (MainUIState != NewNum)
        {
            MainUIState = NewNum;
            emit mainUIStateChanged(); // Сигнал для MainWindow.qml (Глобальные переходы)
        }
    }

    Q_INVOKABLE void setWorkState(int NewNum)
    {
        if (WorkState != NewNum) {
            WorkState = NewNum;
            emit workStateChanged(); // Сигнал для внутренней рабочей зоны
        }
    }

    Q_INVOKABLE void setSecuritySuccessfully(bool NewState)
    {
        if (SecuritySuccessfully != NewState) {
            SecuritySuccessfully = NewState;
            emit securitySuccessfullyChanged();
        }
    }

    bool isCreateChannelDialogOpen() const { return m_isCreateChannelDialogOpen; }
    void setCreateChannelDialogOpen(bool open) {
        if (m_isCreateChannelDialogOpen != open) {
            m_isCreateChannelDialogOpen = open;
            emit isCreateChannelDialogOpenChanged();
        }
    }
    int getDefaultChannelType() const { return m_defaultChannelType; }
    void setDefaultChannelType(int type) {
        if (m_defaultChannelType != type) {
            m_defaultChannelType = type;
            emit defaultChannelTypeChanged();
        }
    }

    Q_INVOKABLE void openCreateChannelDialog(int initialType) {
        setDefaultChannelType(initialType);
        setCreateChannelDialogOpen(true);
    }

    bool isCreateServerDialogOpen() const { return m_isCreateServerDialogOpen; }
    Q_INVOKABLE void setCreateServerDialogOpen(bool open) {
        if (m_isCreateServerDialogOpen != open) {
            m_isCreateServerDialogOpen = open;
            emit isCreateServerDialogOpenChanged();
        }
    }

    QServerManager* GetServerManager() { if(!_ServerManager) _ServerManager = new QServerManager(nullptr); return _ServerManager; }
    QFriendManager* GetFriendManager() { if(!_FriendManager) _FriendManager = new QFriendManager(nullptr); return _FriendManager; }
    QChatManager* GetChatManager() { if(!_ChatManager) _ChatManager = new QChatManager(nullptr); return _ChatManager; }
    QUsersManager* GetUserManager() { if(!_UserManager) _UserManager = new QUsersManager(nullptr); return _UserManager; }

    Q_INVOKABLE void openPersonalWorkspace()
    {
        // 1. Внутреннее состояние переключаем на Друзья (0)
        setWorkState(0);

        // 2. Восстанавливаем последний открытый приватный чат
        QString lastPrivate = GetChatManager()->getLastPrivateRoomGUID();
        if (!lastPrivate.isEmpty()) {
            GetChatManager()->setActiveRoomGUID(lastPrivate);
        }
    }

signals:
    void mainUIStateChanged(); // Сигнал изменения главного экрана
    void workStateChanged();   // Сигнал изменения рабочей зоны
    void isCreateChannelDialogOpenChanged();
    void defaultChannelTypeChanged();
    void isCreateServerDialogOpenChanged();
    void securitySuccessfullyChanged();

private:
    QUIManager(QObject *parent = nullptr): QObject(parent) {};
    QServerManager* _ServerManager = nullptr;
    QFriendManager* _FriendManager = nullptr;
    QChatManager* _ChatManager = nullptr;
    QUsersManager* _UserManager = nullptr;
};

#endif // UIMANAGER_H
