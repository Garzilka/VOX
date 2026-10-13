#include "Relation.h"

void Relation::fillPendingList(vector<string>& result, unordered_set<string>& source)
{
    result.reserve(source.size());
    result.assign(source.begin(), source.end());
    source.clear();
}

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

        fillPendingList(result_Pendings, _RelationsPendingClients);

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

