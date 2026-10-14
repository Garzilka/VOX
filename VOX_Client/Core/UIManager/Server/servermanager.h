#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H
#pragma once

#include <QObject>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QStringList>
#include <QDebug>
#include "Core/Data/DataIncluds.h"

class QServerManager : public QObject
{
    Q_OBJECT

    friend struct ServerManagerPrivateFriend;

    // Свойства для связывания (биндинга) в QML-интерфейс
    Q_PROPERTY(int State READ getState WRITE setState NOTIFY StateChanged)
    Q_PROPERTY(QVariantList serverList READ getServerList NOTIFY serverListChanged)
    Q_PROPERTY(QVariantList currentChannels READ getCurrentChannels NOTIFY currentChannelsChanged)
    Q_PROPERTY(QStringList currentParticipants READ getCurrentParticipants NOTIFY currentParticipantsChanged)
    Q_PROPERTY(QString activeServerGUID READ getActiveServerGUID WRITE setActiveServerGUID NOTIFY activeServerGUIDChanged)

private:
    int ServerState = 0; // 0 - Голосовой/Текстовый чат | 1 - КанБан | 2 - Документы
    QString m_activeServerGUID;

    //QHash<QString, SServerPassport> m_serverRegistry;
    QStringList m_myServers;

    QSet<QString> m_pendingPassportRequests;
    QTimer* m_batchTimer = nullptr;

    struct SServerFullDetails
    {
        QString ServerGUID;
        QList<SChannelPassport> Channels;
        QStringList ParticipantGUIDs;
    };
    QHash<QString, SServerFullDetails> m_serverDetailsCache;
    QHash<QString, QStringList> m_voiceUsersCache;

    // Текущие списки для ListView в QML
    QVariantList m_currentChannels;
    QStringList m_currentParticipants;
public:
    QHash<QString, QString> m_lastServerChannelCache;
    explicit QServerManager(QObject *parent = nullptr);

    int getState() { return ServerState; }
    QVariantList getServerList() const; // Собирает динамическую QML-модель для левой панели
    QVariantList getCurrentChannels() const { return m_currentChannels; }
    QStringList getCurrentParticipants() const { return m_currentParticipants; }
    QString getActiveServerGUID() const { return m_activeServerGUID; }

    Q_INVOKABLE void setState(int NewNum) {
        if (ServerState != NewNum) {
            ServerState = NewNum;
            emit StateChanged();
        }
    }

    // --- ЛОГИКА ДЛЯ ВЫЗОВА ИЗ QML-КОМПОНЕНТОВ ---

    // Вызывается из левой панели (Repeater) и карточек инвайтов.
    // Возвращает имя сервера из реестра, либо ставит GUID в батч-очередь сети
    Q_INVOKABLE QString getServerName(const QString& serverGUID);
    // Вызывается из карточек инвайтов чата. Мгновенно и безошибочно проверяет членство по плоскому списку
    Q_INVOKABLE bool isUserMemberOfServer(const QString& serverGUID) const;

    // Клик по круглой иконке сервера на левой панели
    Q_INVOKABLE void setActiveServerGUID(const QString& serverGUID);

    Q_INVOKABLE QStringList getVoiceChannelUsers(const QString& channelGUID) const {
        return m_voiceUsersCache.value(channelGUID, QStringList());
    }

    Q_INVOKABLE void joinVoiceChannelFromUI(const QString& serverGUID, const QString& channelGUID);
    Q_INVOKABLE void leaveVoiceChannelFromUI(const QString& serverGUID, const QString& channelGUID);

    void initMyServersList(const QStringList& guids);
    void handleServerPassportsLoaded(const QList<SServerPassport>& passports);
    void setServerDetailsFromServer(const SServerDetailsData& details);

    void handleServerCreated(const SCreateServerData& serverData);
    void handleVoiceUserJoined(const QString& channelGUID, const QString& userGUID, const QString& webRTCToken);
    void handleVoiceUserLeft(const QString& channelGUID, const QString& userGUID);

private:
    void sendBatchPassportRequest();
    void updateUIWithServerDetails(const SServerFullDetails& details);
    void forceRefreshChannels() { emit currentChannelsChanged(); }
    void selectActiveChannelForServer(const QString& serverGUID, const QList<SChannelPassport>& channels);

signals:
    void StateChanged();
    void serverListChanged();
    void currentChannelsChanged();
    void currentParticipantsChanged();
    void activeServerGUIDChanged();
    void serverRegistryUpdated();
    void voiceUsersChanged(const QString& channelGUID);
};

#endif // SERVERMANAGER_H
