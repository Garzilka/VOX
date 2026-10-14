#ifndef CHANNELSDATA_H
#define CHANNELSDATA_H
#include "CoreData.h"

struct SAttachmentData
{
    EAttachmentType Type;
    QString Url;          // Ссылка на картинку, файл или иконку стикера
    QString MetaData;     // Дополнительные данные: размер файла, имя файла или токен инвайта

    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["Type"] = static_cast<int>(Type);
        obj["Url"] = Url;
        obj["MetaData"] = MetaData;
        return obj;
    }

    static SAttachmentData FromJSON(const QJsonObject& obj)
    {
        SAttachmentData data;
        data.Type = static_cast<EAttachmentType>(obj["Type"].toInt());
        data.Url = obj["Url"].toString();
        data.MetaData = obj["MetaData"].toString();
        return data;
    }
};


struct SMessageData : public SBaseMessageData
{
    QString RoomGUID;
    QString SenderGUID;
    QString Message;
    QList<SAttachmentData> Attachments;

    SMessageData(QString _RoomGUID, QString _SenderGUID, QString _Message,
                 QList<SAttachmentData> _Attachments = {}, ETypeOfMessage _TypeMessage = ETypeOfMessage::Message)
        : SBaseMessageData(_TypeMessage), RoomGUID(_RoomGUID), SenderGUID(_SenderGUID),
        Message(_Message), Attachments(_Attachments) {};

    virtual ~SMessageData() {}

    SMessageData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomGUID = InJSON["RoomGUID"].toString();
        SenderGUID = InJSON["SenderGUID"].toString();
        Message = InJSON["Message"].toString();

        if (InJSON.contains("Attachments") && InJSON["Attachments"].isArray())
        {
            QJsonArray arr = InJSON["Attachments"].toArray();
            for (const QJsonValue& val : arr)
            {
                Attachments.append(SAttachmentData::FromJSON(val.toObject()));
            }
        }
    };

    // Сериализация в JSON для отправки на сервер
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomGUID"] = RoomGUID;
        messageObj["SenderGUID"] = SenderGUID;
        messageObj["Message"] = Message;

        QJsonArray attachArray;
        for (const auto& attach : Attachments) {
            attachArray.append(attach.ToJSON());
        }
        messageObj["Attachments"] = attachArray;

        return messageObj;
    }
};

struct SChannelPassport
{
    QString chatUUID;
    int ChatType = ETypeOfChat::DirectText;
    QString Name;
    QString ServerUUID;
    QString ParentUUID;
    QString IconUrl;
    QString Description;
    QList<QString> ParticipantGUIDs;
    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["chatUUID"] = chatUUID;
        obj["ChatType"] = ChatType;
        obj["Name"] = Name;
        obj["ServerUUID"] = ServerUUID;
        obj["ParentUUID"] = ParentUUID;
        obj["IconUrl"] = IconUrl;
        obj["Description"] = Description;
        QJsonArray parts;
        for(const auto& id : ParticipantGUIDs) parts.append(id);
        obj["ParticipantGUIDs"] = parts;
        return obj;
    }
    static SChannelPassport FromJSON(const QJsonObject& obj)
    {
        SChannelPassport data;
        data.chatUUID = obj["chatUUID"].toString();
        data.ChatType = obj["ChatType"].toInt();
        data.Name = obj["Name"].toString();
        data.ServerUUID = obj["ServerUUID"].toString();
        data.ParentUUID = obj["ParentUUID"].toString();
        data.Description = obj["Description"].toString();
        if(obj.contains("ParticipantGUIDs") && obj["ParticipantGUIDs"].isArray())
        {
            QJsonArray arr = obj["ParticipantGUIDs"].toArray();
            for(const QJsonValue& val : arr) data.ParticipantGUIDs.append(val.toString());
        }
        return data;
    }
};

struct SChannelDetails : public SBaseMessageData
{
    QString RoomGUID;
    QList<SMessageData> Messages;

    SChannelDetails(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage) {};

    SChannelDetails(QString _RoomGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage),RoomGUID(_RoomGUID)  {};

    SChannelDetails(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
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
