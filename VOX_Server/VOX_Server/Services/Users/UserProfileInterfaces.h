#pragma once
#include "Client.h"


class IUserProfileCacheService 
{
public:
    virtual void warmup(std::function<void(string userUUID)> Success, string userUUID) = 0;
    virtual bool getUserPassport(const string& responseClientUUID, SUserPassport& newPassport) = 0;
    //virtual void getUserDetails(const string& responseClientUUID, const string& targetClientUUID, SUserPassport& newPassport) = 0;
};

class ISearchUserService {
public:
    virtual void handleSearchUsers(const string& responseClientUUID, const SSearchUsersData& data) = 0;
};

class IDBUserService {
public:
};