#ifndef CHANNELSDATA_H
#define CHANNELSDATA_H
#include "CoreData.h"


struct SChannelHistoryData : public SBaseMessageData
{
    QString RoomGUID;
    QList<SMessageData> Messages;

    SChannelHistoryData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage) {};

    SChannelHistoryData(QString _RoomGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage),RoomGUID(_RoomGUID)  {};

    SChannelHistoryData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomGUID = InJSON["RoomGUID"].toString();
        Messages.clear();

        if (InJSON.contains("Messages") && InJSON["Messages"].isArray())
        {
            QJsonArray arr = InJSON["Messages"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    Messages.append(SMessageData(value.toObject()));
                }
            }
        }
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomGUID"] = RoomGUID;

        QJsonArray arr;
        for(const auto& msg : Messages)
        {
            arr.append(msg.ToJSON());
        }
        messageObj["Messages"] = arr;
        return messageObj;
    }
};

struct SChannelCreateData : public SBaseMessageData
{
    int ChatType = ETypeOfChat::DirectText;
    QString TargetUserGUID;
    QString ServerGUID;
    QString Name;
    QString ParentGUID;
    SChannelPassport ChannelPassport;

    SChannelCreateData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelCreate)
        : SBaseMessageData(_TypeMessage) {};
    SChannelCreateData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ChatType = InJSON["ChatType"].toInt();
        TargetUserGUID = InJSON["TargetUserGUID"].toString();
        ServerGUID = InJSON["ServerGUID"].toString();
        Name = InJSON["Name"].toString();
        ParentGUID = InJSON["ParentGUID"].toString();
        if (InJSON.contains("ChannelPassport") && InJSON["ChannelPassport"].isObject())
        {
            ChannelPassport = SChannelPassport::FromJSON(InJSON["ChannelPassport"].toObject());
        }
    };
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ChatType"] = ChatType;
        messageObj["TargetUserGUID"] = TargetUserGUID;
        messageObj["ServerGUID"] = ServerGUID;
        messageObj["Name"] = Name;
        messageObj["ParentGUID"] = ParentGUID;
        messageObj["ChannelPassport"] = ChannelPassport.ToJSON();
        return messageObj;
    }
};

struct SChatNotificationEvent : public SBaseMessageData
{
    QString ChatGUID;
    QString ServerGUID;   // Помогает клиенту соотнести пинг с сервером без загрузки каналов
    int UnreadCount = 0;
    int MentionCount = 0;
    bool IsMuted = false;

    SChatNotificationEvent(ETypeOfMessage _TypeMessage = static_cast<ETypeOfMessage>(21))
        : SBaseMessageData(_TypeMessage) {};

    SChatNotificationEvent(const QJsonObject& InJSON) : SBaseMessageData(InJSON) {
        ChatGUID = InJSON["ChatGUID"].toString();
        ServerGUID = InJSON["ServerGUID"].toString();
        UnreadCount = InJSON["UnreadCount"].toInt();
        MentionCount = InJSON["MentionCount"].toInt();
        IsMuted = InJSON["IsMuted"].toBool();
    }

    virtual QJsonObject ToJSON() const override {
        QJsonObject obj = SBaseMessageData::ToJSON();
        obj["ChatGUID"] = ChatGUID;
        obj["ServerGUID"] = ServerGUID;
        obj["UnreadCount"] = UnreadCount;
        obj["MentionCount"] = MentionCount;
        obj["IsMuted"] = IsMuted;
        return obj;
    }
};

struct SVoiceStateData : public SBaseMessageData
{
    QString ServerGUID;
    QString ChannelGUID;
    QString UserGUID;
    QString WebRTCToken; // ДОБАВЛЕНО: Токен для прямого UDP-подключения к LiveKit

    SVoiceStateData(ETypeOfMessage _TypeMessage = static_cast<ETypeOfMessage>(22))
        : SBaseMessageData(_TypeMessage) {};

    SVoiceStateData(const QJsonObject& InJSON) : SBaseMessageData(InJSON) {
        ServerGUID = InJSON["ServerGUID"].toString();
        ChannelGUID = InJSON["ChannelGUID"].toString();
        UserGUID = InJSON["UserGUID"].toString();
        WebRTCToken = InJSON["WebRTCToken"].toString(); // Добавлено
    }

    virtual QJsonObject ToJSON() const override {
        QJsonObject obj = SBaseMessageData::ToJSON();
        obj["ServerGUID"] = ServerGUID;
        obj["ChannelGUID"] = ChannelGUID;
        obj["UserGUID"] = UserGUID;
        obj["WebRTCToken"] = WebRTCToken; // Добавлено
        return obj;
    }
};

#endif // CHANNELSDATA_H
