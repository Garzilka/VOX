#pragma once
#include "../Common.h"
#include "../../Libs/Tools/ToolLib.h"

//TODO:переделать на namespace string-ов
enum class ETypeOfMessage : uint8_t
{
    Ping            = 0, // -
    Crypto          = 1, // ++
    Message         = 2, // 
    Disconnect      = 3, // -
    AppVersion      = 4, // + 

    Login           = 5, // ++
    Registration    = 6, // ++
    AuthToken       = 7, // -

    ChannelCreate   = 8,  // + Вернуть UUID после создания | Вернуть UUID если существует
    ChannelPassport = 9,  // + UUID, Имя, Последнее сообщение, кому принадлежит 
    ChannelDetails  = 10, // + История сообщений 
    ChannelJoin     = 11, // + 
    ChannelLeave    = 12, // +
    ChannelNotify   = 13, // -

    SearchUsers     = 14, // +
    Relations       = 15, // +
    FriendRequest   = 16, // +
    FriendAccept    = 17, // +

    ServerCreate    = 18, // +
    ServerInvite    = 19, // +
    ServerAccept    = 20, // +
    ServerPassport  = 21, // + UUID, Имя, URL иконки
    ServerDetails   = 22, // +  Роли, Участники, UUID каналов

    UserPassport    = 23, // + UUID, Имя, URL иконки, список UUID серверов 
    UserDetails     = 24, // - О себе, URL обложки, Рамка 
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

