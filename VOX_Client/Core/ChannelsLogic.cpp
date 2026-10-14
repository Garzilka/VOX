#include "connection.h"
#include "Core/Data/DataIncluds.h"
#include "Core/UIManager/uimanager.h"




void QConnection::requestHistoryMessage(const QString &UUID)
{
    SChannelDetails msg(UUID);
    sendMessage(msg);
}

void QConnection::ResponseOpenOrCreateChat(QJsonObject &JSONObject)
{
    SCreateServerData data(JSONObject);
    // Передаем GUID активной комнаты в менеджер чатов
    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr) chatMgr->setActiveRoomGUID("");
    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr) friendMgr->setState(1); // Переключаем вкладку на текстовый чат
    QUIManager::getInstance().setMainUIState(0);
}

void QConnection::ResponseHistoryChannel(QJsonObject &JSONObject)
{
    SChannelDetails historyData(JSONObject);
    QVariantList formattedMessages;

    for (const SMessageData& msg : historyData.Messages)
    {
        QVariantMap msgMap;
        msgMap["sender"] = msg.SenderGUID;
        msgMap["messageText"] = msg.Message;
        msgMap["time"] = "12:00"; // Здесь в будущем привяжешь реальное время из БД
        msgMap["avatarColor"] = "#5865f2";

        // =========================================================================
        // ИСПРАВЛЕНО: Маппинг массива вложений (Attachments) для ListView истории чата
        // =========================================================================
        QVariantList attachmentsList;
        for (const SAttachmentData& attach : msg.Attachments)
        {
            QVariantMap attachMap;
            attachMap["type"] = static_cast<int>(attach.Type);
            attachMap["url"] = attach.Url;
            attachMap["metaData"] = attach.MetaData;
            attachmentsList.append(attachMap);
        }
        msgMap["attachments"] = attachmentsList; // Прокидываем массив вложений в сообщение

        formattedMessages.append(msgMap);
    }

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr) {
        chatMgr->setHistoryFromServer(historyData.RoomGUID, formattedMessages);
    }
}

void QConnection::ResponseDirectChats(QJsonObject &JSONObject)
{
    /*qDebug() << "CONNECTION LOG | Получен ответ ResponseDirectChats от сервера. JSON:" << JSONObject;

    SRequestDirectChatsData data(JSONObject);
    qDebug() << "CONNECTION LOG | Распаковано чатов из пакета:" << data.Chats.size();

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr)
    {
        chatMgr->clearDirectChats();

        for (const SUnifiedChatData& chat : data.Chats)
        {
            chatMgr->addDirectChat(chat);
        }

        qDebug() << "CONNECTION LOG | Хэш-таблица заполнена. Вызываем сигналы оповещения UI.";
        emit directChatsLoaded(chatMgr->getDirectChatsList());
        emit chatMgr->directChatsChanged();
    } else
    {
        qCritical() << "CONNECTION LOG ERROR | Не удалось получить экземпляр QChatManager через QUIManager!";
    }*/
}

void QConnection::requestOpenOrCreateChat(int chatType, const QString& targetUserGUID, const QString& serverGUID, const QString& name)
{
   /*SOpenOrCreateChatData msg;
    msg.ChatType = chatType;
    msg.TargetUserGUID = targetUserGUID;
    msg.ServerGUID = serverGUID;
    msg.Name = name;
    sendMessage(msg);*/
}

void QConnection::requestDirectChats()
{
    /*SRequestDirectChatsData msg;
    sendMessage(msg);*/
}
