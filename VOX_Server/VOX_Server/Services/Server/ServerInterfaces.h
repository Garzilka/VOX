#pragma once
#include "Data/MessagesIncluds.h"


class IServerService {
public:
    virtual void makeServer(const string userUUID, const SCreateServerData& Req) = 0;
    virtual void ServerInvite(const string userUUID, const SGenerateInviteData& Req) = 0;
    virtual void ServerAccept(const string userUUID, const SAcceptInviteData& obj) = 0;
};

class ICacheServerService 
{
public:
    virtual bool getServerPassport(const string& userUUID , SServerPassport& Result) = 0;
    virtual bool getServerDetails(const string& userUUID, SServerDetailsData& Result) = 0;
    virtual bool setServerPassport(SServerPassport in_Data) = 0;
};