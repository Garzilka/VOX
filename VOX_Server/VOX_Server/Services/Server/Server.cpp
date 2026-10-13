#include "Server.h"

void Server::fillPendingList(vector<string>& result, unordered_set<string>& source) 
{
    result.reserve(source.size());
    result.assign(source.begin(), source.end());
    source.clear();
}

bool Server::setServerPassport(SServerPassport newPassport, vector<string>& result_Pendings)
{
    std::lock_guard<mutex> lock(_ServerMutex);

    _ServerPassport = std::move(newPassport);;
    _isValid = !_ServerPassport.Name.empty() && !_ServerPassport.ServerUUID.empty();

    if (!_isValid) return false;

    fillPendingList(result_Pendings, _PassportPendingClients);

    return true;
}

bool Server::setServerDetails(const SServerDetailsData& newServerDetailsData, vector<string>& result_Pendings)
{
    std::lock_guard<mutex> lock(_ServerMutex);

    _ServerDetailsData = std::move(newServerDetailsData);;
    _isDetailsLoaded = true;

    fillPendingList(result_Pendings, _DetailsPendingClients);

    return true;
}

void Server::addPendingClients(const string& clientUUID)
{
    std::lock_guard<mutex> lock(_ServerMutex);
    _PassportPendingClients.insert(clientUUID);
}

bool Server::isDetailsNeedLoaded() const
{
    std::lock_guard<mutex> lock(_ServerMutex);
    return _DetailsPendingClients.size() > 0;
}

vector<string> Server::getMembers()
{
    std::lock_guard<mutex> lock(_ServerMutex);
    vector<string> Result;

    for (const auto& clientUUID : _ServerDetailsData.MembersData)
    {
        Result.push_back(clientUUID.UserUUID);
    }
    return Result;
}


bool Server::getServerPassport(SServerPassport& result_Passport, const string& clientUUID)
{
    if (!isValid())
    {
        std::lock_guard<mutex> lock(_ServerMutex);

        _PassportPendingClients.insert(clientUUID);
        return false;
    }

    result_Passport = _ServerPassport;
    return true;
}


bool Server::getServerDetails(SServerDetailsData& result_ServerDetailsData, const string& clientUUID)
{
    std::lock_guard<mutex> lock(_ServerMutex);
    if (!(isValid() && isDetailsLoaded()))
    {
        _DetailsPendingClients.insert(clientUUID);
        return false;
    }

    result_ServerDetailsData = _ServerDetailsData;
    return true;
}