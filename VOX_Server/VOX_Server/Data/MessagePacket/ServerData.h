#pragma once
#include "CoreData.h"


struct SGenerateInviteData : public SBaseMessageData
{
    string ServerUUID;
    string TargetUserUUID; // СКОРРЕКТИРОВАНО: ID получателя вместо ID комнаты
    string Token;

    SGenerateInviteData(string _ServerUUID, string _TargetUserUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerInvite)
        : SBaseMessageData(_TypeMessage), ServerUUID(_ServerUUID), TargetUserUUID(_TargetUserUUID) {};

    SGenerateInviteData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerUUID      =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        TargetUserUUID  =ToolLib::jsonGetValue<string>(InJSON, "TargetUserUUID");
        Token           =ToolLib::jsonGetValue<string>(InJSON, "Token");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerUUID"]        = ServerUUID;
        messageObj["TargetUserUUID"]    = TargetUserUUID;
        messageObj["Token"]             = Token;
        return messageObj;
    }
};

struct SAcceptInviteData : public SBaseMessageData
{
    string Token;
    string ServerUUID;
    SAcceptInviteData(string _Token, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerAccept)
        : SBaseMessageData(_TypeMessage), Token(_Token) {};
    SAcceptInviteData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Token =ToolLib::jsonGetValue<string>(InJSON, "Token");
        ServerUUID =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
    };
    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Token"] = Token;
        messageObj["ServerUUID"] = ServerUUID;
        return messageObj;
    }
};

struct SCreateServerData : public SBaseMessageData
{
    string ServerName;
    string OwnerUUID;
    string ServerUUID;
    SCreateServerData(string _ServerName, string _OwnerUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerCreate)
        : SBaseMessageData(_TypeMessage), ServerName(_ServerName), OwnerUUID(_OwnerUUID) {};

    SCreateServerData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerName      =ToolLib::jsonGetValue<string>(InJSON, "ServerName");
        OwnerUUID       =ToolLib::jsonGetValue<string>(InJSON, "OwnerUUID");
        ServerUUID      =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerName"] = ServerName;
        messageObj["OwnerUUID"] = OwnerUUID;
        messageObj["ServerUUID"] = ServerUUID;
        return messageObj;
    }
};

struct SServerPassport
{
    string ServerUUID{""};
    string Name{""};
    string IconUrl{""};

    SServerPassport() = default;

    JsonObject ToJSON() const 
    {
        JsonObject obj;
        obj["ServerUUID"] = ServerUUID;
        obj["Name"] = Name;
        obj["IconUrl"] = IconUrl;
        return obj;
    }

    static SServerPassport FromJSON(const JsonObject& InJSON)
    {
        SServerPassport info;
        info.ServerUUID     =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        info.Name           =ToolLib::jsonGetValue<string>(InJSON, "Name");
        info.IconUrl        =ToolLib::jsonGetValue<string>(InJSON, "IconUrl");
        return info;
    }
};

inline void to_json(JsonObject& j, const SServerPassport& d) {
    j = d.ToJSON();
}

inline void from_json(const JsonObject& j, SServerPassport& d) {
    d = SServerPassport::FromJSON(j);
}

struct SServerPassportFor : public SBaseMessageData
{
    string serverUUID;
    SServerPassport Server;

    SServerPassportFor(ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerPassport)
        : SBaseMessageData(_TypeMessage) {};

    // Умный конструктор парсинга JSON, читающий оба массива
    SServerPassportFor(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        serverUUID = InJSON["serverUUID"];
        Server = SServerPassport::FromJSON(InJSON["Server"]);
    };

    // Сериализация структуры в JSON
    virtual JsonObject ToJSON() const override 
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["serverUUID"] = serverUUID;
        messageObj["Server"] = Server;

        return messageObj;
    }
};

struct SServerRoleData
{
    string RoleUUID;
    string Name;
    string Color;
    int64_t Permissions = 0;
    int16_t Position = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SServerRoleData, RoleUUID, Name, Color, Permissions, Position)
};

struct SServerMemberShortData
{
    string UserUUID;
    string RoleUUID; // Может быть пустой строкой, если роли нет

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SServerMemberShortData, UserUUID, RoleUUID)
};

struct SServerDetailsData : public SBaseMessageData
{
    string ServerUUID{""};
    vector<string> Channels{};
    vector<SServerRoleData> RoleData{};
    vector<SServerMemberShortData> MembersData{};

    SServerDetailsData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerDetails) : SBaseMessageData(_TypeMessage) {};

    SServerDetailsData(string _ServerUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerDetails)
        : SBaseMessageData(_TypeMessage), ServerUUID(_ServerUUID) {};

    SServerDetailsData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerUUID =ToolLib::jsonGetValue<string>(InJSON, "ServerUUID");
        Channels.clear();
        RoleData.clear();
        MembersData.clear();

        if (InJSON.contains("Channels") && InJSON["Channels"].is_array())
        {
            const auto& arr = InJSON["Channels"];
            Channels.reserve(arr.size());
            for (const auto& item : arr)
            {
                Channels.push_back(item);
            }
        }

        if (InJSON.contains("RoleData") && InJSON["RoleData"].is_array())
        {
            const auto& arr = InJSON["RoleData"];
            RoleData.reserve(arr.size());
            for (const auto& item : arr)
            {
                RoleData.push_back(item);
            }
        }

        if (InJSON.contains("MembersData") && InJSON["MembersData"].is_array())
        {
            const auto& arr = InJSON["MembersData"];
            MembersData.reserve(arr.size());
            for (const auto& item : arr)
            {
                MembersData.push_back(item);
            }
        }
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerUUID"] = ServerUUID;
        messageObj["Channels"] = Channels;
        messageObj["RoleData"] = RoleData;
        messageObj["MembersData"] = MembersData;
        return messageObj;
    }
};
