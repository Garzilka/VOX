#pragma once
#include "../Common.h"
#include "../../Libs/Tools/ToolLib.h"

//TODO:переделать на namespace string-ов
enum class ETypeOfMessage : uint8_t
{
    Ping            = 0, //
    Crypto          = 1, // +
    Message         = 2, //
    Disconnect      = 3, //
    AppVersion      = 4, // + 

    Login           = 5, // +
    Registration    = 6, // +
    AuthToken       = 7, //

    ChannelCreate   = 8,  //Вернуть UUID после создания | Вернуть UUID если существует
    ChannelPassport = 9,  //UUID, Имя, Последнее сообщение, кому принадлежит 
    ChannelDetails  = 10, //История сообщений 
    ChannelJoin     = 11,
    ChannelLeave    = 12,
    ChannelNotify   = 13,

    SearchUsers     = 14, //
    Friends         = 15,
    Friendships     = 16,
    FriendRequest   = 17,
    FriendAccept    = 18,

    ServerCreate    = 19,
    ServerInvite    = 20,
    ServerAccept    = 21,
    ServerPassport  = 22, //UUID, Имя, URL иконки
    ServerDetails   = 23, //Роли, Участники, UUID каналов

    UserPassport    = 24, //+ UUID, Имя, URL иконки 
    UserDetails     = 25, //О себе, URL обложки, Рамка, список UUID серверов
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
    bool Response = true;

    SBaseMessageData(ETypeOfMessage type, bool response = true)
        : TypeMessage(type), Response(response) {}

    virtual ~SBaseMessageData() = default;

    SBaseMessageData(JsonObject InJSON)
    {
        Response =ToolLib::jsonGetValue<bool>(InJSON, "Response");
        TypeMessage = static_cast<ETypeOfMessage>(ToolLib::jsonGetValue<int>(InJSON, "TypeMessage"));
    };

    virtual JsonObject ToJSON() const 
    {
        JsonObject j;
        j["TypeMessage"] = static_cast<int>(TypeMessage);
        j["Response"] = Response;
        return j;
    }
};

struct SCyptsKeysData : public SBaseMessageData
{
    string PublicKey;

    SCyptsKeysData(string _PublicKey, ETypeOfMessage _TypeMessage = ETypeOfMessage::Crypto)
        : SBaseMessageData(_TypeMessage), PublicKey(_PublicKey) {
    };

    SCyptsKeysData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        PublicKey =ToolLib::jsonGetValue<string>(InJSON, "PublicKey");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["PublicKey"] = PublicKey;
        return messageObj;
    }
};

//Временная реализация, потом нарежется на куски
struct SAuthorizationData : public SBaseMessageData
{
    //Клиент/Сервер
    string longToken; //LongLifeToken

    //Клиент
    string Login;
    string Password;
    string Email;
    bool RememberMe = false;

    //Сервер
    string UUID; //Ответ сервера
    SAuthorizationData() : SBaseMessageData(ETypeOfMessage::Login, false) {}

    SAuthorizationData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        longToken   =ToolLib::jsonGetValue<string>(InJSON, "longToken");
        Login       =ToolLib::jsonGetValue<string>(InJSON, "Login");
        Password    =ToolLib::jsonGetValue<string>(InJSON, "Password");
        Email       =ToolLib::jsonGetValue<string>(InJSON, "Email");
        RememberMe  =ToolLib::jsonGetValue<bool>(InJSON, "RememberMe");
        UUID        =ToolLib::jsonGetValue<string>(InJSON, "UUID");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        
        messageObj["longToken"]     = longToken;
        messageObj["Login"]         = Login;
        messageObj["Password"]      = Password;
        messageObj["Email"]         = Email;
        messageObj["RememberMe"]    = RememberMe;
        messageObj["UUID"]          = UUID;
        return messageObj;
    }
};

struct SAttachmentData 
{
    EAttachmentType Type;
    string Url;
    string MetaData;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SAttachmentData, Type, Url, MetaData)
};

struct SMessageData : public SBaseMessageData
{
    string RoomUUID = "";
    string SenderUUID = "";
    string Message = "";
    vector<SAttachmentData> Attachments = {};

    SMessageData(string _RoomUUID, string _SenderUUID, string _Message, ETypeOfMessage _TypeMessage = ETypeOfMessage::Message) 
        : SBaseMessageData(_TypeMessage), RoomUUID(_RoomUUID), SenderUUID(_SenderUUID),
        Message(_Message) {};

    virtual ~SMessageData() {}

    explicit SMessageData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomUUID        =ToolLib::jsonGetValue<string>(InJSON, "RoomUUID", "");
        SenderUUID      =ToolLib::jsonGetValue<string>(InJSON, "SenderUUID", "");
        Message         =ToolLib::jsonGetValue<string>(InJSON, "Message", "");

        if (InJSON.contains("Attachments") && InJSON["Attachments"].is_array())
        {
            Attachments = InJSON["Attachments"].get<vector<SAttachmentData>>();
        }
    }

    static SMessageData FromJSON(const JsonObject& InJSON)
    {
        return SMessageData(InJSON);
    }

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomUUID"] = RoomUUID;
        messageObj["SenderUUID"] = SenderUUID;
        messageObj["Message"] = Message;
        messageObj["Attachments"] = Attachments;

        return messageObj;
    }
};
inline void to_json(JsonObject& j, const SMessageData& d) { j = d.ToJSON(); }
inline void from_json(const JsonObject& j, SMessageData& d){  d.FromJSON(j);}

struct SUserPassport 
{
    string UUID;
    string DisplayName;
    string AvatarUrl;
    int PresenceStatus = EUserPresenceStatus::Offline;
    int RelationStatus = EUserRelationType::None;

    SUserPassport() {};

    SUserPassport(const JsonObject& InJSON)
    {
        UUID            = ToolLib::jsonGetValue<string>(InJSON, "UUID");
        DisplayName     = ToolLib::jsonGetValue<string>(InJSON, "DisplayName");
        AvatarUrl       = ToolLib::jsonGetValue<string>(InJSON, "AvatarUrl");
        PresenceStatus  = ToolLib::jsonGetValue<int>(InJSON, "PresenceStatus");
        RelationStatus  = ToolLib::jsonGetValue<int>(InJSON, "RelationStatus");
    }

    JsonObject ToJSON() const
    {
        JsonObject messageObj;
        messageObj["UUID"] = UUID;
        messageObj["DisplayName"] = DisplayName;
        messageObj["AvatarUrl"] = AvatarUrl;
        messageObj["PresenceStatus"] = PresenceStatus;
        messageObj["RelationStatus"] = RelationStatus;

        return messageObj;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SUserPassport, UUID, DisplayName, AvatarUrl, PresenceStatus, RelationStatus)
};

struct SUserDetails
{
    string aboutMe;
    string bannerUrl;
    string avatarFrameUrl;
    unordered_set<std::string> joinedServersUUID;
};

struct SUserDetails
{
    string aboutMe;
    string bannerUrl;
    string avatarFrameUrl;
};

struct SPeerCustomSettings
{
    std::string note;         
    int voiceVolume = 100;    
    bool isMuted = false;     
};
struct SUserRelations
{
    string userOwnerUUID;
    unordered_set<string> friendsUUID;
    unordered_set<string> bannedUUID; 
    unordered_map<string, SPeerCustomSettings> peerSettings;
};
struct SChannelPassport
{
    string ChatUUID;
    int ChatType = ETypeOfChat::DirectText;
    string Name;        
    string ServerUUID;  
    string ParentUUID;  
    string Description;
    vector<string> ParticipantUUIDs; 

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SChannelPassport,
        ChatUUID, ChatType, Name, ServerUUID, ParentUUID,
        Description, ParticipantUUIDs
    )
};


struct SAppVersionData : public SBaseMessageData
{
    //Клиент прислал
    int ClientVersionCode = INDEX_NONE;
    string Platform{ "" };

    //Сервер ответил
    int LatestVersionCode = INDEX_NONE;
    string LatestVersionName{""};
    string DownloadUrl{ "" };
    string Sha256Hash{ "" };

    SAppVersionData(ETypeOfMessage _TypeMessage = static_cast<ETypeOfMessage>(24))
        : SBaseMessageData(_TypeMessage) {};

    SAppVersionData(const JsonObject& InJSON) : SBaseMessageData(InJSON) 
    {
        ClientVersionCode   =ToolLib::jsonGetValue<int>(InJSON, "ClientVersionCode");
        Platform            =ToolLib::jsonGetValue<string>(InJSON, "Platform");
        LatestVersionCode   =ToolLib::jsonGetValue<int>(InJSON, "LatestVersionCode");
        LatestVersionName   =ToolLib::jsonGetValue<string>(InJSON, "LatestVersionName");
        DownloadUrl         =ToolLib::jsonGetValue<string>(InJSON, "DownloadUrl");
        Sha256Hash          =ToolLib::jsonGetValue<string>(InJSON, "Sha256Hash");
    }

    virtual JsonObject ToJSON() const override 
    {
        JsonObject obj = SBaseMessageData::ToJSON();
        obj["ClientVersionCode"] = ClientVersionCode;
        obj["Platform"] = Platform;
        obj["LatestVersionCode"] = LatestVersionCode;
        obj["LatestVersionName"] = LatestVersionName;
        obj["DownloadUrl"] = DownloadUrl;
        obj["Sha256Hash"] = Sha256Hash;
        return obj;
    }
};
