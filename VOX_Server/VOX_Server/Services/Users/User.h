#pragma once

#include "../../Data/Common.h"
#include "../../Data/MessagesIncluds.h"

class User
{

private:
    SUserPassport _UserPassport;
    unordered_set<string> _PassportPendingClients;
    vector<std::function<void(string userUUID)>> Callbacks;
    mutable mutex _ServerMutex;

    bool _isValid = false;

    bool LoadDetails();
public:
    explicit User() = default;
    bool getUserPassport(SUserPassport& newPassport, const string& clientUUID);

    void warmup(std::function<void(string userUUID)>& Callback);
    bool setUserPassport(SUserPassport newUserPasport, vector<string>& result_Pendings);
    void addPendingClients(const string& clientUUID);

    bool isValid() const { return _isValid; }
};