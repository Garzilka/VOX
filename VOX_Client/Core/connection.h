#ifndef CONNECTION_H
#define CONNECTION_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>
#include <QVariantMap>
#include <QTime>
#include "Core/Data/CoreData.h"
#include "Core/Lib/cryptolib.h"

class QConnection : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *m_socket;
    QCryptoLib* cryptolib = nullptr;
    QByteArray m_buffer;

    bool DebugMode = false;
    QString UUID = "";
    bool g_shouldRememberNextAuth = false;
    explicit QConnection(QObject *parent = nullptr);
    ~QConnection() { }

public:
    QConnection& operator=(const QConnection&) = delete;
    QConnection(const QConnection&) = delete;

    static QConnection& getInstance()
    {
        static QConnection instance(nullptr);
        return instance;
    };

    Q_INVOKABLE QString getUUID() { return UUID; };
    // В класс QConnection в секцию public:
    Q_INVOKABLE void checkSavedSession(); // Вызывать при старте QML
    Q_INVOKABLE void logout();            // Для сброса "Запомни меня"

    void connectToServer(const QString &host, quint16 port);
    bool sendMessage(const SBaseMessageData& message);

    /* ============== REQUESTS (Вызываются из QML/UI менеджеров) ============== */
    Q_INVOKABLE void requestAuthorization(const QString &Login, const QString &Password, bool rememberMe = false);
    Q_INVOKABLE void requestRegistration(const QString &Login, const QString &Email, const QString &Password, bool rememberMe = false);
    Q_INVOKABLE void requestSearchUser(const QString &UserName);
    Q_INVOKABLE void requestFriend(const QString &GUID, const QString &UserName);
    Q_INVOKABLE void requestFriendAction(const QString& targetGUID, int action);
    Q_INVOKABLE void requestJoinVoice(const QString &serverGUID, const QString &channelGUID);
    Q_INVOKABLE void requestLeaveVoice(const QString &serverGUID, const QString &channelGUID);

    // Новые ленивые запросы
    Q_INVOKABLE void requestDirectChats();                              // Запрос DM-списка
    Q_INVOKABLE void requestServerDetails(const QString& serverGUID);   // Клик на круглую иконку сервера
    Q_INVOKABLE void requestOpenDataUsers(const QList<QString>& guids);    // Пакетный запрос просроченных профилей
    Q_INVOKABLE void requestOpenOrCreateChat(int chatType, const QString& targetUserGUID, const QString& serverGUID, const QString& name);
    Q_INVOKABLE void requestCreateServer(const QString& serverName);
    Q_INVOKABLE void requestGenerateInvite(const QString& serverGUID, const QString& targetUserGUID);
    Q_INVOKABLE void requestAcceptInvite(const QString& token);
    Q_INVOKABLE void requestHistoryMessage(const QString& UUID);
    /* ====================================================================== */

signals:
    void authErrorReceived(const QString &errorText);

    // Сигналы ленивой загрузки для распределения данных в UI-менеджеры
    void directChatsLoaded(const QVariantList& chats);
    void serverDetailsLoaded(const QString& serverGUID, const QVariantList& channels, const QStringList& userGUIDs);
    void batchUsersLoaded(const QVariantList& users);
    void serverCreated(const QString& serverGUID, const QString& serverName, const QString& generalChatGUID);
    void inviteGenerated(const QString& token, const QString& serverGUID, const QString& roomGUID);
    void inviteAccepted(const QString& serverGUID, const QString& serverName, const QString& generalChatGUID);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    int ReadCurrentPacket(QJsonObject& Result);

    /* ============== RESPONSE (Обработка входящих пакетов от сервера) ============== */
    void NewMessage(QJsonObject& JSONObject);

    void ResponseCrypto(QJsonObject& JSONObject);
    void ResponseAuth(QJsonObject& JSONObject);
    void ResponseSearchUsers(QJsonObject& JSONObject);
    void ResponseFriendsList(QJsonObject& JSONObject);
    void ResponseFriendships(QJsonObject& JSONObject);
    void ResponseHistoryChannel(QJsonObject& JSONObject);
    void ResponseVoiceUserLeft(QJsonObject &JSONObject);
    void ResponseVoiceUserJoined(QJsonObject &JSONObject);

    // Новые ленивые ответы
    void ResponseDirectChats(QJsonObject& JSONObject);
    void ResponseServerDetails(QJsonObject& JSONObject);
    void ResponseOpenUsersData(QJsonObject& JSONObject);
    void ResponseOpenOrCreateChat(QJsonObject& JSONObject);
    void ResponseCreateServer(QJsonObject& JSONObject);
    void ResponseGenerateInvite(QJsonObject& JSONObject);
    void ResponseAcceptInvite(QJsonObject& JSONObject);
    void ResponseServersList(QJsonObject& JSONObject);
    void ResponseServerShortData(QJsonObject &JSONObject);
    /* ============================================================================== */
};

#endif // CONNECTION_H
