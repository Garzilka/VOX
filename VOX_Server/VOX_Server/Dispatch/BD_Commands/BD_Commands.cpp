#include "BD_Commands.h"
#include "Data/MessagesIncluds.h"

void CmdCreateServer::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SMakeServerTask InData = SMakeServerTask::FromJSON(data.payload);

    SServerPassport ServerPassport;
    bool Response = _Repository->createServerInDB(InData, ServerPassport);
    if (!Response)
    {
        Exit(data);
        return;
    }

    SMakeServerResponse Result = SMakeServerResponse(InData.userUUID, ServerPassport);
    data.onResult(Response, Result.ToJSON());
}

void CmdRequestServerPassport::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SServerPassport ServerPassport = SServerPassport::FromJSON(data.payload);

    bool Response = _Repository->loadServerPassport(ServerPassport);
    if (!Response)
    {
        Exit(data);
        return;
    }

    data.onResult(Response, ServerPassport.ToJSON());
}

void CmdRequestServerDetails::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SServerDetailsData InData = SServerDetailsData(data.payload);

    if (!_Repository->loadServerChannelsUUIDs(InData.ServerUUID, InData.Channels))
    {
        Exit(data);
        return;
    }

    if (!_Repository->loadServerMembersWithRoles(InData.ServerUUID, InData.MembersData))
    {
        Exit(data);
        return;
    }

    if (!_Repository->loadServerRoles(InData.ServerUUID, InData.RoleData))
    {
        Exit(data);
        return;
    }

    data.onResult(true, InData.ToJSON());
}

void CmdRequestAppVersion::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SAppVersionData InData = SAppVersionData(data.payload);

    bool Response = _Repository->loadAppVersionData(InData);
    if (!Response)
    {
        Exit(data);
        return;
    }

    data.onResult(Response, InData.ToJSON());
}

void CmdRequestLogin::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SAuthClientRequest InData = SAuthClientRequest(data.payload);

    string Response = _Repository->CheckAuth(InData._AuthReq);
    if (Response.empty())
    {
        Exit(data);
        return;
    }

    SAuthClientResponse Result;
    Result._UserpeerAddress = InData._UserpeerAddress;
    Result._AuthReponse = InData._AuthReq;
    Result._AuthReponse.UUID = Response;
    data.onResult(true, Result.ToJSON());
}

void CmdRequestReg::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SAuthClientRequest InData = SAuthClientRequest(data.payload);

    string Response = _Repository->RegisterNewAccount(InData._AuthReq);
    if (Response.empty())
    {
        Exit(data);
        return;
    }

    SAuthClientResponse Result;
    Result._UserpeerAddress = InData._UserpeerAddress;
    Result._AuthReponse = InData._AuthReq;
    Result._AuthReponse.UUID = Response;
    data.onResult(true, Result.ToJSON());
}

void CmdLoadRelation::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SUserRelations Result = SUserRelations(data.payload);
    try
    {
        if (Result.userOwnerUUID.empty())
        {
            Exit(data);
            return;
        }

        bool result = _Repository->getUserRelations(Result.userOwnerUUID, Result);
        data.onResult(result, Result.ToJSON());
    }
    catch (const std::exception& e)
    {
        cerr << "CmdLoadRelation EXECUTE ERROR | " << e.what() << endl;
        Exit(data);
    }
}

void CmdFriendRequest::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SFriendRequest InData = SFriendRequest(data.payload);

    auto Response = _Repository->sendFriendRequest(InData.SenderUUID, InData.TargetUUID);
    if (!Response.first)
    {
        Exit(data);
        return;
    }

    data.onResult(true, InData.ToJSON());
}

void CmdFriendAccept::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SFriendAccept InData = SFriendAccept(data.payload);

    auto Response = _Repository->handleFriendResponse(InData.SenderUUID, InData.TargetUUID, InData.Action);
    if (!Response)
    {
        Exit(data);
        return;
    }

    data.onResult(true, InData.ToJSON());
}

void CmdRequestUserPassport::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SUserPassport InData = SUserPassport(data.payload);

    auto Response = _Repository->loadUserPassport(InData);
    if (!Response)
    {
        Exit(data);
        return;
    }

    data.onResult(true, InData.ToJSON());
}

void CmdSearchUsers::execute(const BDTask& data)
{
    if (!_Repository)
    {
        Exit(data);
        return;
    }
    SSearchUserBD InData = SSearchUserBD(data.payload);

    auto Response = _Repository->searchUsersByUsername(InData.userUUID, InData._SearchUsersData);
    if (!Response)
    {
        Exit(data);
        return;
    }

    data.onResult(true, InData.ToJSON());
}
