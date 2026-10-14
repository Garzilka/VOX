#pragma once
#include "CoreData.h"


struct SSearchUsersData : public SBaseMessageData
{
    string SearchQuery;
    vector<string> Results;
    SSearchUsersData(string _SearchQuery = "", ETypeOfMessage _TypeMessage = ETypeOfMessage::SearchUsers)
        : SBaseMessageData(_TypeMessage), SearchQuery(_SearchQuery) {
    };

    SSearchUsersData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SearchQuery = ToolLib::jsonGetValue<string>(InJSON, "SearchQuery");
        Results.clear();

        if (InJSON.contains("Results") && InJSON["Results"].is_array())
        {
            Results = InJSON["Results"];
        }
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SearchQuery"] = SearchQuery;
        messageObj["Results"] = Results;
        return messageObj;
    }
};

struct SUserPassport : public SBaseMessageData
{
    string UUID;
    string DisplayName;
    string AvatarUrl;
    unordered_set<string> joinedServersUUID;
    int PresenceStatus = EUserPresenceStatus::Offline;

    SUserPassport() : SBaseMessageData(ETypeOfMessage::UserPassport) {};

    SUserPassport(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        UUID = ToolLib::jsonGetValue<string>(InJSON, "UUID");
        DisplayName = ToolLib::jsonGetValue<string>(InJSON, "DisplayName");
        AvatarUrl = ToolLib::jsonGetValue<string>(InJSON, "AvatarUrl");
        PresenceStatus = ToolLib::jsonGetValue<int>(InJSON, "PresenceStatus");
        joinedServersUUID.clear();
        if (InJSON.contains("joinedServersUUID") && InJSON["joinedServersUUID"].is_array())
        {
            for (const auto& item : InJSON["joinedServersUUID"])
            {
                if (item.is_string())
                {
                    joinedServersUUID.insert(item.get<string>());
                }
            }
        }
    }

    JsonObject ToJSON() const
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["UUID"] = UUID;
        messageObj["DisplayName"] = DisplayName;
        messageObj["AvatarUrl"] = AvatarUrl;
        messageObj["PresenceStatus"] = PresenceStatus;
        messageObj["joinedServersUUID"] = joinedServersUUID;

        return messageObj;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SUserPassport, UUID, DisplayName, AvatarUrl, PresenceStatus, joinedServersUUID)
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

    SPeerCustomSettings(const JsonObject& InJSON)
    {
        note = ToolLib::jsonGetValue<string>(InJSON, "note");
        voiceVolume = ToolLib::jsonGetValue<int>(InJSON, "voiceVolume");
        isMuted = ToolLib::jsonGetValue<bool>(InJSON, "isMuted");
    }

    JsonObject ToJSON() const
    {
        JsonObject messageObj;
        messageObj["note"] = note;
        messageObj["voiceVolume"] = voiceVolume;
        messageObj["isMuted"] = isMuted;

        return messageObj;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SPeerCustomSettings, note, voiceVolume, isMuted)
};

struct SUserRelations : public SBaseMessageData
{
    string userOwnerUUID;
    unordered_set<string> IncRequestFriend;
    unordered_set<string> OutcRequestFrined;
    unordered_set<string> friendsUUID;
    unordered_set<string> bannedUUID;
    unordered_map<string, SPeerCustomSettings> peerSettings;

    SUserRelations(ETypeOfMessage _TypeMessage = ETypeOfMessage::Relations)
        : SBaseMessageData(_TypeMessage) {
    };

    SUserRelations(const JsonObject& InJSON, ETypeOfMessage _TypeMessage = ETypeOfMessage::Relations)
        : SBaseMessageData(_TypeMessage)
    {
        userOwnerUUID = ToolLib::jsonGetValue<string>(InJSON, "userOwnerUUID");

        if (InJSON.contains("IncRequestFriend") && InJSON["IncRequestFriend"].is_array())
        {
            IncRequestFriend = InJSON["IncRequestFriend"].get<unordered_set<string>>();
        }

        if (InJSON.contains("OutcRequestFrined") && InJSON["OutcRequestFrined"].is_array())
        {
            OutcRequestFrined = InJSON["OutcRequestFrined"].get<unordered_set<string>>();
        }

        if (InJSON.contains("friendsUUID") && InJSON["friendsUUID"].is_array())
        {
            friendsUUID = InJSON["friendsUUID"].get<unordered_set<string>>();
        }

        if (InJSON.contains("bannedUUID") && InJSON["bannedUUID"].is_array())
        {
            bannedUUID = InJSON["bannedUUID"].get<unordered_set<string>>();
        }

        /*if (InJSON.contains("peerSettings") && InJSON["peerSettings"].is_object())
        {
            peerSettings = InJSON["peerSettings"].get<unordered_map<string, SPeerCustomSettings>>();
        }*/
    }

    JsonObject ToJSON() const
    {
        JsonObject messageObj;
        messageObj["IncRequestFriend"] = IncRequestFriend;
        messageObj["OutcRequestFrined"] = OutcRequestFrined;
        messageObj["userOwnerUUID"] = userOwnerUUID;
        messageObj["friendsUUID"] = friendsUUID;
        messageObj["bannedUUID"] = bannedUUID;
        //messageObj["peerSettings"] = peerSettings;

        return messageObj;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SUserRelations, userOwnerUUID, friendsUUID, bannedUUID, peerSettings)
};