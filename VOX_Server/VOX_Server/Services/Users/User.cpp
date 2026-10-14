#include "User.h"


bool User::getUserPassport(SUserPassport& newPassport, const string& clientUUID)
{
    if (!isValid())
    {
        std::lock_guard<mutex> lock(_ServerMutex);

        _PassportPendingClients.insert(clientUUID);
        return false;
    }

    newPassport = _UserPassport;
    return true;

}

void User::warmup(std::function<void(string userUUID)>& Callback)
{
    vector<std::function<void(string)>> localCallbacks;
    string userUUID = "";

    {
        std::lock_guard<mutex> lock(_ServerMutex);

        Callbacks.push_back(Callback);
        if (!isValid()) return;

        localCallbacks = Callbacks;
        userUUID = _UserPassport.UUID;
        Callbacks.clear();
    }

    for (const auto& callback : localCallbacks)
    {
        if (callback) callback(userUUID);
    }
}

bool User::setUserPassport(SUserPassport newUserPasport, vector<string>& result_Pendings)
{
    vector<std::function<void(string)>> localCallbacks;
    string userUUID = "";

    {
        std::lock_guard<mutex> lock(_ServerMutex);

        _UserPassport = std::move(newUserPasport);;
        _isValid = !_UserPassport.DisplayName.empty() && !_UserPassport.UUID.empty();

        if (!_isValid) return false;

        ToolLib::fillPendingList(result_Pendings, _PassportPendingClients);

        userUUID = _UserPassport.UUID;
        localCallbacks = Callbacks;
        Callbacks.clear();
    }

    if (userUUID.empty()) return _isValid;

    for (const auto& callback : localCallbacks)
    {
        if (callback) callback(userUUID);
    }

    return true;
}
