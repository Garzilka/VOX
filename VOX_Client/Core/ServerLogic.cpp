#include "connection.h"
#include "Core/Data/DataIncluds.h"
#include "Core/UIManager/uimanager.h"
#include "Core/UIManager/Server/servermanager.h"


void QConnection::ResponseCreateServer(QJsonObject &JSONObject)
{
    /*SCreateServerData data(JSONObject);
    QUIManager::getInstance().GetServerManager()->handleServerCreated(data);
    emit serverCreated(data.ServerGUID, data.ServerName, data.GeneralChatGUID);*/
}

void QConnection::ResponseGenerateInvite(QJsonObject &JSONObject)
{
    SGenerateInviteData data(JSONObject);
}

void QConnection::ResponseAcceptInvite(QJsonObject &JSONObject)
{

}

void QConnection::ResponseServersList(QJsonObject &JSONObject)
{

}

void QConnection::ResponseServerShortData(QJsonObject &JSONObject)
{
    /*SServersList Data(JSONObject);

    qInfo() << "Транспорт | Получен ленивый пакет паспортов серверов. Количество:" << Data.Servers.size();

    // Передаем массив паспортов в клиентский ServerManager для укладывания в кэш реестра
    QServerManager* serverMgr = QUIManager::getInstance().GetServerManager();
    if (serverMgr)
    {
        serverMgr->handleServerPassportsLoaded(Data.Servers);
    }*/
}
void QConnection::requestGenerateInvite(const QString& serverGUID, const QString& targetUserGUID)
{
    SGenerateInviteData msg(serverGUID, targetUserGUID);
    sendMessage(msg);
}


void QConnection::requestJoinVoice(const QString &serverGUID, const QString &channelGUID)
{
    SVoiceStateData msg(ETypeOfMessage::ChannelJoin); // RequestJoinVoice
    msg.ServerGUID = serverGUID;
    msg.ChannelGUID = channelGUID;
    sendMessage(msg);
}

void QConnection::requestLeaveVoice(const QString &serverGUID, const QString &channelGUID)
{
    SVoiceStateData msg(ETypeOfMessage::ChannelLeave);
    msg.ServerGUID = serverGUID;
    msg.ChannelGUID = channelGUID;
    sendMessage(msg);
}

void QConnection::ResponseVoiceUserJoined(QJsonObject &JSONObject)
{
    SVoiceStateData data(JSONObject);

    QServerManager* serverMgr = QUIManager::getInstance().GetServerManager();
    if (serverMgr)
    {
        serverMgr->handleVoiceUserJoined(data.ChannelGUID, data.UserGUID, data.WebRTCToken);
    }
}

void QConnection::ResponseVoiceUserLeft(QJsonObject &JSONObject)
{
    SVoiceStateData data(JSONObject);

    QServerManager* serverMgr = QUIManager::getInstance().GetServerManager();
    if (serverMgr) {
        serverMgr->handleVoiceUserLeft(data.ChannelGUID, data.UserGUID);
    }
}
void QConnection::requestAcceptInvite(const QString& token)
{
    SAcceptInviteData msg(token);
    sendMessage(msg);
}

void QConnection::requestCreateServer(const QString& serverName)
{
    SCreateServerData msg(serverName, UUID);
    sendMessage(msg);
}
void QConnection::requestServerDetails(const QString& serverGUID)
{
    /*SServerDetailsData msg(serverGUID);
    sendMessage(msg);*/
}

void QConnection::ResponseServerDetails(QJsonObject &JSONObject)
{
    /*SServerDetailsData data(JSONObject);

    // 2. Стучимся в QServerManager через глобальный синглтон UI-менеджера
    // (Убедись, что у тебя в QUIManager заложен геттер GetServerManager(), по аналогии с GetChatManager)
    QServerManager* serverMgr = QUIManager::getInstance().GetServerManager();
    if (serverMgr)
    {
        // Передаем распарсенный контейнер напрямую в бизнес-логику менеджера
        serverMgr->setServerDetailsFromServer(data);
        qDebug() << "Транспорт | Ленивые детали сервера" << data.ServerGUID << "успешно переданы в QServerManager.";
    }

    // 3. Дублируем данные в старый сигнальный поток (если QML подписан на сигналы напрямую)
    QVariantList formattedChannels;
    QStringList participants;

    for (const SUnifiedChatData& chan : data.Channels)
    {
        QVariantMap chanMap;
        chanMap["chatGUID"] = chan.ChatGUID;
        chanMap["chatType"] = chan.ChatType;
        chanMap["name"] = chan.Name;
        chanMap["parentGUID"] = chan.ParentGUID;
        formattedChannels.append(chanMap);
    }
    for (const QString& id : data.UserGUIDs) participants.append(id);

    emit serverDetailsLoaded(data.ServerGUID, formattedChannels, participants);*/
}
