#ifndef COREDATA_H
#define COREDATA_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

enum class ETypeOfMessage : uint8_t
{
    Ping            = 0,
    Crypto          = 1,
    Message         = 2,
    Disconnect      = 3,
    AppVersion      = 4,

    Login           = 5,
    Registration    = 6,
    AuthToken       = 7,

    ChannelCreate   = 8,  //Вернуть UUID после создания | Вернуть UUID если существует
    ChannelsPrivate = 9,
    ChannelPassport = 10,  //UUID, Имя, Последнее сообщение, кому принадлежит
    ChannelDetails  = 11, //История сообщений
    ChannelJoin     = 12,
    ChannelLeave    = 13,
    ChannelNotify   = 14,

    SearchUsers     = 15,
    Friends         = 16,
    Friendships     = 17,
    FriendRequest   = 18,
    FriendAccept    = 19,

    ServerCreate    = 20,
    ServerList      = 21,
    ServerInvite    = 22,
    ServerAccept    = 23,
    ServerPassport  = 24, //UUID, Имя, URL иконки
    ServerDetails   = 25, //Роли, Участники, UUID каналов

    UserPassport    = 26, //UUID, Имя, URL иконки
    UserDetails     = 27, //О себе, URL обложки, Рамка
};


enum EUserRelationType
{
    None = -1,           // Никто (просто найден в поиске или согильдиец)
    PendingSent = 0,     // Мы отправили заявку в друзья
    PendingReceived = 1, // Нам отправили заявку в друзья
    Friends = 2,         // Мы друзья
    Blocked = 3          // Пользователь заблокирован нами
};

enum EUserPresenceStatus
{
    Offline = 0,
    Online = 1,
    Idle = 2,         // Неактивен / Отошел (HidedOnline)
    NotDisturb = 3,   // Не беспокоить (DND)
    Ninja = 4,        // Отображаться как Offline
};

enum ETypeOfChat
{
    DirectText = 0,
    GroupText = 1,
    ServerText = 2,
    DirectVoice = 3,
    GroupVoice = 4,
    ServerVoice = 5
};
enum EAttachmentType
{
    AttachmentImg = 0,
    AttachmentFile = 1,
    AttachmentSticker = 2,
    AttachmentServerInvite = 3
};
struct SBaseMessageData
{
    ETypeOfMessage TypeMessage;
    bool Response = false;

    SBaseMessageData(ETypeOfMessage _TypeMessage, bool _Response = true)
        : TypeMessage(_TypeMessage), Response(_Response)
    {};

    SBaseMessageData(QJsonObject InJSON)
    {
        Response = InJSON["Response"].toBool();
        TypeMessage = static_cast<ETypeOfMessage>(InJSON["TypeMessage"].toInt());
    };

    virtual QJsonObject ToJSON() const
    {
        QJsonObject messageObj;
        messageObj["TypeMessage"] = static_cast<int>(TypeMessage);
        messageObj["Response"] = Response;
        return messageObj;
    }
};

struct SCyptsKeysData : public SBaseMessageData
{
    QString PublicKey;

    SCyptsKeysData(QString _PublicKey, ETypeOfMessage _TypeMessage = ETypeOfMessage::Crypto)
        : SBaseMessageData(_TypeMessage), PublicKey(_PublicKey) {};

    SCyptsKeysData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        PublicKey = InJSON["PublicKey"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["PublicKey"] = PublicKey;
        return messageObj;
    }
};

struct SAuthorizationData : public SBaseMessageData
{
    //Клиент/Сервер
    QString longToken; //LongLifeToken

    //Клиент
    QString Login;
    QString Password;
    QString Email;
    bool RememberMe = false;

    //Сервер
    QString UUID; //Ответ сервера

    SAuthorizationData(SBaseMessageData BaseData, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(BaseData), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(ETypeOfMessage _TypeMessage, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(_TypeMessage), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        longToken = InJSON["longToken"].toString();
        Login = InJSON["Login"].toString();
        Email = InJSON["Email"].toString();
        RememberMe = InJSON["RememberMe"].toBool();
        UUID = InJSON["UUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["longToken"] = longToken;
        messageObj["Login"] = Login;
        messageObj["Password"] = Password;
        messageObj["Email"] = Email;
        messageObj["RememberMe"] = RememberMe;
        return messageObj;
    }
};

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

// Обновленная структура сообщения
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

    // Парсинг входящего JSON от сервера
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

struct SUserPassport
{
    QString UUID;
    QString DisplayName;
    QString AvatarUrl;
    int PresenceStatus = EUserPresenceStatus::Offline;
    int RelationStatus = EUserRelationType::None;

    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["UUID"] = UUID;
        obj["DisplayName"] = DisplayName;
        obj["AvatarUrl"] = AvatarUrl;
        obj["PresenceStatus"] = PresenceStatus;
        obj["RelationStatus"] = RelationStatus;
        return obj;
    }

    static SUserPassport FromJSON(const QJsonObject& obj)
    {
        SUserPassport data;
        data.UUID = obj["UUID"].toString();
        data.DisplayName = obj["DisplayName"].toString();
        data.AvatarUrl = obj["AvatarUrl"].toString();
        data.PresenceStatus = obj["PresenceStatus"].toInt();
        data.RelationStatus = obj["RelationStatus"].toInt();
        return data;
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


struct SAppVersionData : public SBaseMessageData
{
    int ClientVersionCode = -1;
    QString Platform; // 'windows', 'linux', 'macos'

    // Поля ответа (заполняются сервером)
    int LatestVersionCode = -1;
    QString LatestVersionName;
    QString DownloadUrl;
    QString Sha256Hash;

    SAppVersionData(ETypeOfMessage _TypeMessage = static_cast<ETypeOfMessage>(24))
        : SBaseMessageData(_TypeMessage) {};

    SAppVersionData(const QJsonObject& InJSON) : SBaseMessageData(InJSON) {
        ClientVersionCode = InJSON["ClientVersionCode"].toInt();
        Platform = InJSON["Platform"].toString();
        LatestVersionCode = InJSON["LatestVersionCode"].toInt();
        LatestVersionName = InJSON["LatestVersionName"].toString();
        DownloadUrl = InJSON["DownloadUrl"].toString();
        Sha256Hash = InJSON["Sha256Hash"].toString();
    }

    virtual QJsonObject ToJSON() const override {
        QJsonObject obj = SBaseMessageData::ToJSON();
        obj["ClientVersionCode"] = ClientVersionCode;
        obj["Platform"] = Platform;
        obj["LatestVersionCode"] = LatestVersionCode;
        obj["LatestVersionName"] = LatestVersionName;
        obj["DownloadUrl"] = DownloadUrl;
        obj["Sha256Hash"] = Sha256Hash;
        return obj;
    }
};



#endif // COREDATA_H
