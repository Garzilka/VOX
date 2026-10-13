#pragma once
#include "Client.h"


class IUserProfileCacheService 
{
public:
    virtual void warmup(std::function<void(string userUUID)> Success, string userUUID) = 0;
    virtual void getUserPassport(const string& responseClientUUID, const string& targetClientUUID, SUserPassport& newPassport) = 0;
    virtual void getUserDetails(const string& responseClientUUID, const string& targetClientUUID, SUserPassport& newPassport) = 0;
};

class IUserService {
public:
    void handleSearchUsers(shared_ptr<Client> client, const JsonObject& data);
};

class IDBUserService {
public:
};