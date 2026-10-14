#pragma once
#include "CoreData.h"


struct SFriendRequest : public SBaseMessageData
{
    string SenderUUID;
    string TargetUUID;

    SFriendRequest(string _SenderUUID, string _TargetUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendRequest)
        : SBaseMessageData(_TypeMessage), SenderUUID(_SenderUUID), TargetUUID(_TargetUUID) {};

    SFriendRequest(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SenderUUID      =ToolLib::jsonGetValue<string>(InJSON, "SenderUUID");
        TargetUUID      =ToolLib::jsonGetValue<string>(InJSON, "TargetUUID");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SenderUUID"] = SenderUUID;
        messageObj["TargetUUID"] = TargetUUID;
        return messageObj;
    }
};

struct SFriendAccept : public SBaseMessageData
{
    string SenderUUID;
    string TargetUUID;
    bool Action = false;        // 0 - Принять, 1 - Отклонить/Удалить

    SFriendAccept(string _senderUUID, string TargetUUID, bool _Action, ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendAccept)
        : SBaseMessageData(_TypeMessage), SenderUUID(_senderUUID), TargetUUID(TargetUUID), Action(_Action) {};

    SFriendAccept(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        TargetUUID  =ToolLib::jsonGetValue<string>(InJSON, "TargetUUID");
        Action      =ToolLib::jsonGetValue<bool>(InJSON, "Action");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["TargetUUID"] = TargetUUID;
        messageObj["Action"] = Action;
        return messageObj;
    }
};
