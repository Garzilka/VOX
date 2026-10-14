#pragma once
#include "CoreData.h"


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
        Message(_Message) {
    };

    virtual ~SMessageData() {}

    explicit SMessageData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomUUID = ToolLib::jsonGetValue<string>(InJSON, "RoomUUID", "");
        SenderUUID = ToolLib::jsonGetValue<string>(InJSON, "SenderUUID", "");
        Message = ToolLib::jsonGetValue<string>(InJSON, "Message", "");

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
inline void from_json(const JsonObject& j, SMessageData& d) { d.FromJSON(j); }





struct SChannelPassport : public SBaseMessageData
{
    string ChatUUID;
    int ChatType = ETypeOfChat::DirectText;
    string Name;
    string ServerUUID;
    string ParentUUID;
    string Description;
    vector<string> ParticipantUUIDs;
    SChannelPassport() : SBaseMessageData(ETypeOfMessage::ChannelPassport) {};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SChannelPassport,
        ChatUUID, ChatType, Name, ServerUUID, ParentUUID,
        Description, ParticipantUUIDs
    )
};

struct SChannelDetails : public SBaseMessageData
{
    string RoomUUID;
    vector<SMessageData> Messages;

    SChannelDetails(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage) {
    };

    SChannelDetails(string _RoomUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelDetails)
        : SBaseMessageData(_TypeMessage), RoomUUID(_RoomUUID) {
    };

    SChannelDetails(const JsonObject& InJSON) : SBaseMessageData(InJSON)
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
    string ChannelUUID;
    string UserUUID;
    string WebRTCToken;

    SChannelJoin(ETypeOfMessage _TypeMessage = ETypeOfMessage::ChannelJoin)
        : SBaseMessageData(_TypeMessage) {};

    SChannelJoin(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ChannelUUID =ToolLib::jsonGetValue<string>(InJSON, "ChannelUUID");
        UserUUID =ToolLib::jsonGetValue<string>(InJSON, "UserUUID");
        WebRTCToken =ToolLib::jsonGetValue<string>(InJSON, "WebRTCToken");
    }

    virtual JsonObject ToJSON() const override 
    {
        JsonObject obj = SBaseMessageData::ToJSON();
        obj["ChannelUUID"] = ChannelUUID;
        obj["UserUUID"] = UserUUID;
        obj["WebRTCToken"] = WebRTCToken;
        return obj;
    }
};
