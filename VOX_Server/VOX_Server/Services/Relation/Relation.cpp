#include "Relation.h"


bool Relation::getUserRelations(SUserRelations& outRelations, const string& clientUUID)
{
    std::lock_guard<mutex> lock(_ServerMutex);

    if (!isValid())
    {
        _RelationsPendingClients.insert(clientUUID);
        return false;
    }

    outRelations = _UserRelations;
    return true;
}

void Relation::addNewFriend(const string& targetClientUUID)
{
    std::lock_guard<mutex> lock(_ServerMutex);
    _UserRelations.IncRequestFriend.erase(targetClientUUID);
    _UserRelations.OutcRequestFrined.erase(targetClientUUID);
    _UserRelations.friendsUUID.insert(targetClientUUID);
}

void Relation::addRequestFriend(const string& targetClientUUID)
{
    std::lock_guard<mutex> lock(_ServerMutex);
    _UserRelations.friendsUUID.erase(targetClientUUID);
    if (targetClientUUID == _UserRelations.userOwnerUUID)
    {
        _UserRelations.IncRequestFriend.insert(targetClientUUID);
    }
    else
    {
        _UserRelations.OutcRequestFrined.insert(targetClientUUID);
    }
}

void Relation::removeFriend(const string& targetClientUUID)
{
    _UserRelations.friendsUUID.erase(targetClientUUID);
}

void Relation::removeRequestFriend(const string& targetClientUUID)
{
    _UserRelations.IncRequestFriend.erase(targetClientUUID);
    _UserRelations.OutcRequestFrined.erase(targetClientUUID);
}


void Relation::warmup(std::function<void(string userUUID)>& Callback)
{
    vector<std::function<void(string)>> localCallbacks;
    string userUUID = "";

    {
        std::lock_guard<mutex> lock(_ServerMutex);

        Callbacks.push_back(Callback);
        if (!isValid()) return;

        localCallbacks = Callbacks;
        userUUID = _UserRelations.userOwnerUUID;
        Callbacks.clear();
    }

    for (const auto& callback : localCallbacks)
    {
        if (callback) callback(userUUID);
    }
}

bool Relation::setUserRelations(SUserRelations newUserelations, vector<string>& result_Pendings)
{
    vector<std::function<void(string)>> localCallbacks;
    string userUUID = "";

    {
        std::lock_guard<mutex> lock(_ServerMutex);

        _UserRelations = std::move(newUserelations);
        userUUID = _UserRelations.userOwnerUUID;

        ToolLib::fillPendingList(result_Pendings, _RelationsPendingClients);

        localCallbacks = Callbacks;
        Callbacks.clear();
    }

    if (userUUID.empty()) return false;

    for (const auto& callback : localCallbacks)
    {
        if (callback) callback(userUUID);
    }

    return true;
}

