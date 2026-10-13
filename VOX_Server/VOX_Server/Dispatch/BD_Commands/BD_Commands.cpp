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
    SRequestByServerTask InData = SRequestByServerTask::FromJSON(data.payload);

    SServerPassport ServerPassport;
    ServerPassport.ServerUUID = InData.serverUUID;
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
    SRequestByServerTask InData = SRequestByServerTask::FromJSON(data.payload);

    SServerDetailsData ServerDetails;
    ServerDetails.ServerUUID = InData.serverUUID;
    ServerDetails.Channels.clear();
    ServerDetails.MembersData.clear();
    ServerDetails.RoleData.clear();

    if (!_Repository->loadServerChannelsUUIDs(InData.serverUUID, ServerDetails.Channels))
    {
        Exit(data);
        return;
    }

    if (!_Repository->loadServerMembersWithRoles(InData.serverUUID, ServerDetails.MembersData))
    {
        Exit(data);
        return;
    }

    if (!_Repository->loadServerRoles(InData.serverUUID, ServerDetails.RoleData))
    {
        Exit(data);
        return;
    }

    data.onResult(true, ServerDetails.ToJSON());
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
