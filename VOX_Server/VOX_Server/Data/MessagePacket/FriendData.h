#pragma once
#include "CoreData.h"

struct SRequestUserPassport : public SBaseMessageData
{
    string userUUID; // Что просит клиент
    SUserPassport userPassport; // Что возвращает сервер

    SRequestUserPassport(ETypeOfMessage _TypeMessage = ETypeOfMessage::UserPassport)
        : SBaseMessageData(_TypeMessage) {};

    SRequestUserPassport(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        userUUID = InJSON["userUUID"];
        userPassport = InJSON["userPassport"];
    };
    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["userUUID"] = userUUID;
        messageObj["userPassport"] = userPassport;
        return messageObj;
    }
};

struct SSearchUsersData : public SBaseMessageData
{
    string SearchQuery;
    vector<SUserPassport> Results;
    SSearchUsersData(string _SearchQuery, ETypeOfMessage _TypeMessage = ETypeOfMessage::SearchUsers)
        : SBaseMessageData(_TypeMessage), SearchQuery(_SearchQuery) {};
    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SearchQuery"] = SearchQuery;
        messageObj["Results"] = Results;
        return messageObj;
    }
    SSearchUsersData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SearchQuery =ToolLib::jsonGetValue<string>(InJSON, "SearchQuery");
        Results.clear();

        if (InJSON.contains("Results") && InJSON["Results"].is_array())
        {
            Results = InJSON["Results"];
        }
    };
};


struct SFriendshipsResponse : public SBaseMessageData
{
    vector<SUserPassport> Requests;
    SFriendshipsResponse(ETypeOfMessage _TypeMessage = ETypeOfMessage::Friendships)
        : SBaseMessageData(_TypeMessage) {};
    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Requests"] = Requests;
        return messageObj;
    }
    SFriendshipsResponse(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Requests.clear();
        if (InJSON.contains("Requests") && InJSON["Requests"].is_array()) 
        {
            Requests = InJSON["Requests"];
        }
    };
};

struct SFriendRequestData : public SBaseMessageData
{
    string SenderUUID;
    string TargetUsername;
    string TargetUUID;

    SFriendRequestData(string _SenderUUID, string _TargetUsername, string _TargetUUID = "", ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendRequest)
        : SBaseMessageData(_TypeMessage), SenderUUID(_SenderUUID), TargetUsername(_TargetUsername), TargetUUID(_TargetUUID) {};

    SFriendRequestData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SenderUUID      =ToolLib::jsonGetValue<string>(InJSON, "SenderUUID");
        TargetUsername  =ToolLib::jsonGetValue<string>(InJSON, "TargetUsername");
        TargetUUID      =ToolLib::jsonGetValue<string>(InJSON, "TargetUUID");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SenderUUID"] = SenderUUID;
        messageObj["TargetUsername"] = TargetUsername;
        messageObj["TargetUUID"] = TargetUUID;
        return messageObj;
    }
};

struct SFriendResponseData : public SBaseMessageData
{
    string TargetUUID;
    bool Action = false;        // 0 - Принять, 1 - Отклонить/Удалить

    SFriendResponseData(string _TargetUUID, bool _Action, ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendAccept)
        : SBaseMessageData(_TypeMessage), TargetUUID(_TargetUUID), Action(_Action) {};

    SFriendResponseData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
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

struct SFriendsListData : public SBaseMessageData
{
    vector<string> Friends;

    SFriendsListData(ETypeOfMessage _TypeMessage = ETypeOfMessage::Friends)
        : SBaseMessageData(_TypeMessage) {};

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Friends"] = Friends;
        return messageObj;
    }

    SFriendsListData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Friends.clear();
        if (InJSON.contains("Friends") && InJSON["Friends"].is_array())
        {
            Friends = InJSON["Friends"];
        }
    };
};
