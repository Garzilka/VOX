#pragma once
#include "Data/Common.h"
#include "Libs/Tools/ToolLib.h"
#include "../Data/MessagesIncluds.h"

enum class EBDTaskType
{
    None,
    LoadUserPassport,
    LoadUserDetails,
    CreateServer,
    ServerInvite,
    ServerAccept,
    LoadServerPassport,
    LoadServerDetails,
    GetServerList,
    UpdateStatus,
    LoadAppVersion,
    Login,
    TokenAuth,
    Registration,
    LoadRelation,
    FriendRequest,
    FriendAccept,
    SearchUser,
    ChannelCreate,
    LoadChannelPassport,
    LoadChannelDetails,
    ChannelJoin,
    ChannelLeave,
};

struct BDTask
{
    EBDTaskType type;
    string targetUuid;      // Ключ, по которому ищем объект в памяти
    JsonObject payload;      // Входные данные (имя, конфиг и т.д.)
    BDTask(EBDTaskType result_Type = EBDTaskType::None) : type(result_Type) {};
    // Callback: вызывается в потоке БД, когда запрос готов.
    // success = true, если всё ок, false если ошибка БД.
    std::function<void(bool success, const JsonObject& res)> onResult;
};

struct SMakeServerTask
{
    string userUUID{ "" };
    string Name{ "" };
    string IconUrl{ "" };

    SMakeServerTask() = default;
    SMakeServerTask(const string& result_userUUID, const string& result_Name) : userUUID(result_userUUID), Name(result_Name) {};

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["userUUID"] = userUUID;
        obj["Name"] = Name;
        obj["IconUrl"] = IconUrl;
        return obj;
    }

    static SMakeServerTask FromJSON(const JsonObject& InJSON)
    {
        SMakeServerTask info;
        info.userUUID = ToolLib::jsonGetValue<string>(InJSON, "userUUID");
        info.Name = ToolLib::jsonGetValue<string>(InJSON, "Name");
        info.IconUrl = ToolLib::jsonGetValue<string>(InJSON, "IconUrl");
        return info;
    }
};

struct SMakeServerResponse
{
    string userUUID{ "" };
    SServerPassport _ServerPassport;

    SMakeServerResponse() = default;
    SMakeServerResponse(string result_userUUID, SServerPassport result_ServerPassport) : userUUID(std::move(result_userUUID)), _ServerPassport(std::move(result_ServerPassport)) {};

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["userUUID"] = userUUID;
        obj["ServerPassport"] = _ServerPassport.ToJSON();
        return obj;
    }

    static SMakeServerResponse FromJSON(const JsonObject& InJSON)
    {
        SMakeServerResponse info;
        info.userUUID =ToolLib::jsonGetValue<string>(InJSON, "userUUID");
        info._ServerPassport = SServerPassport::FromJSON(InJSON["_ServerPassport"]);
        return info;
    }
};

struct SRequestByServerTask
{
    string serverUUID{ "" };

    SRequestByServerTask() = default;
    SRequestByServerTask(const string& result_userUUID) : serverUUID(result_userUUID) {};

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["serverUUID"] = serverUUID;
        return obj;
    }

    static SRequestByServerTask FromJSON(const JsonObject& InJSON)
    {
        SRequestByServerTask info;
        info.serverUUID =ToolLib::jsonGetValue<string>(InJSON, "serverUUID");
        return info;
    }
};

struct SAuthClientRequest
{
    string _UserpeerAddress;
    SAuthorizationData _AuthReq;

    SAuthClientRequest() {};

    SAuthClientRequest(const JsonObject& Obj) : _AuthReq(Obj["_AuthorizationData"])
    {
        _UserpeerAddress = ToolLib::jsonGetValue<string>(Obj, "_UserpeerAddress");
    }

    SAuthClientRequest(string in_UserpeerAddress, SAuthorizationData in_AuthorizationData) :
        _UserpeerAddress(std::move(in_UserpeerAddress)), _AuthReq(std::move(in_AuthorizationData)) {};

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["_UserpeerAddress"] = _UserpeerAddress;
        obj["_AuthorizationData"] = _AuthReq.ToJSON();
        return obj;
    }
};

struct SAuthClientResponse
{
    string _UserpeerAddress;
    SAuthorizationData _AuthReponse;

    SAuthClientResponse() {};

    SAuthClientResponse(const JsonObject& Obj) : _AuthReponse(Obj["_AuthorizationData"])
    {
        _UserpeerAddress =ToolLib::jsonGetValue<string>(Obj, "_UserpeerAddress");
    }

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["_UserpeerAddress"] = _UserpeerAddress;
        obj["_AuthorizationData"] = _AuthReponse.ToJSON();
        return obj;
    }
};

struct SSearchUserBD
{
    string userUUID;
    SSearchUsersData _SearchUsersData;

    SSearchUserBD() {};

    SSearchUserBD(const JsonObject& Obj) : _SearchUsersData(Obj["SearchUsersData"])
    {
        userUUID = ToolLib::jsonGetValue<string>(Obj, "userUUID");
    }

    JsonObject ToJSON() const
    {
        JsonObject obj;
        obj["userUUID"] = userUUID;
        obj["SearchUsersData"] = _SearchUsersData.ToJSON();
        return obj;
    }
};
