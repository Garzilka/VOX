#pragma once
#include "CoreData.h"


struct SChannelHistoryData : public SBaseMessageData
{
    string RoomUUID;
    vector<SMessageData> Messages;

    SChannelHistoryData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage) {
    };

    SChannelHistoryData(string _RoomUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage), RoomUUID(_RoomUUID) {
    };

    SChannelHistoryData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomUUID = ToolLib::jsonGetValue<string>(InJSON, "RoomUUID");
        Messages.clear();

        /*if (InJSON.contains("Messages") && InJSON["Messages"].is_array())
        {
            Messages = InJSON["Messages"];
        }*/
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomUUID"] = RoomUUID;
        messageObj["Messages"] = Messages;
        return messageObj;
    }
};

struct SChannelCreateData : public SBaseMessageData
{
    int ChatType = ETypeOfChat::DirectText;
    string TargetUserUUID;
    string ServerUUID;
    string Name;
    string ParentUUID;
    SChannelPassport ChannelPassport;
    SChannelCreateData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelCreate)
        : SBaseMessageData(_TypeMessage) {};
    SChannelCreateData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ChatType        =ToolLib::jsonGetValue<int>(InJSON, "ChatType");
        TargetUserUUID  =ToolLib::jsonGetValue<string>(InJSON, "TargetUserUUID");
        ServerUUID      =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        Name            =ToolLib::jsonGetValue<string>(InJSON, "Name");
        ParentUUID      =ToolLib::jsonGetValue<string>(InJSON, "ParentUUID");

        if (InJSON.contains("ChannelPassport")) 
        {
            ChannelPassport = InJSON["ChannelPassport"].get<SChannelPassport>();
        }
    };
    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ChatType"] = ChatType;
        messageObj["TargetUserUUID"] = TargetUserUUID;
        messageObj["ServerUUID"] = ServerUUID;
        messageObj["Name"] = Name;
        messageObj["ParentUUID"] = ParentUUID;
        messageObj["ChannelPassport"] = ChannelPassport;
        return messageObj;
    }
};

struct SChatNotificationEvent : public SBaseMessageData
{
    string ChatUUID;
    string ServerUUID;   // Помогает клиенту соотнести пинг с сервером без загрузки каналов
    int UnreadCount = 0;
    int MentionCount = 0;
    bool IsMuted = false;

    SChatNotificationEvent(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelNotify)
        : SBaseMessageData(_TypeMessage) {};

    SChatNotificationEvent(const JsonObject& InJSON) : SBaseMessageData(InJSON) 
    {
        ChatUUID =ToolLib::jsonGetValue<string>(InJSON, "ChatUUID");
        ServerUUID =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        UnreadCount =ToolLib::jsonGetValue<int>(InJSON, "UnreadCount");
        MentionCount =ToolLib::jsonGetValue<int>(InJSON, "MentionCount");
        IsMuted =ToolLib::jsonGetValue<bool>(InJSON, "IsMuted");
    }

    virtual JsonObject ToJSON() const override 
    {
        JsonObject obj = SBaseMessageData::ToJSON();
        obj["ChatUUID"] = ChatUUID;
        obj["ServerUUID"] = ServerUUID;
        obj["UnreadCount"] = UnreadCount;
        obj["MentionCount"] = MentionCount;
        obj["IsMuted"] = IsMuted;
        return obj;
    }
};

struct SChannelJoin : public SBaseMessageData
{
    string ServerUUID;
    string ChannelUUID;
    string UserUUID;
    string WebRTCToken; // ДОБАВЛЕНО: Токен для прямого UDP-подключения к LiveKit

    SChannelJoin(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelJoin)
        : SBaseMessageData(_TypeMessage) {};

    SChannelJoin(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerUUID =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        ChannelUUID =ToolLib::jsonGetValue<string>(InJSON, "ChannelUUID");
        UserUUID =ToolLib::jsonGetValue<string>(InJSON, "UserUUID");
        WebRTCToken =ToolLib::jsonGetValue<string>(InJSON, "WebRTCToken");
    }

    virtual JsonObject ToJSON() const override 
    {
        JsonObject obj = SBaseMessageData::ToJSON();
        obj["ServerUUID"] = ServerUUID;
        obj["ChannelUUID"] = ChannelUUID;
        obj["UserUUID"] = UserUUID;
        obj["WebRTCToken"] = WebRTCToken; // Добавлено
        return obj;
    }
};
