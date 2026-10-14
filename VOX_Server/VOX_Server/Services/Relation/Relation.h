#pragma once

#include "../../Data/Common.h"
#include "../../Data/MessagesIncluds.h"

class Relation
{

private:
    SUserRelations _UserRelations;
    unordered_set<string> _RelationsPendingClients;
    vector<std::function<void(string userUUID)>> Callbacks;
    mutable mutex _ServerMutex;

    bool _isValid = false;

    bool LoadDetails();
public:
    explicit Relation() = default;

    bool setUserRelations(SUserRelations newUserRelations, vector<string>& result_Pendings);
    bool getUserRelations(SUserRelations& outRelations, const string& clientUUID);
    void addNewFriend(const string& targetClientUUID);
    void addRequestFriend(const string& targetClientUUID);
    void addPendingClients(const string& clientUUID);
    virtual void removeFriend(const string& targetClientUUID);
    virtual void removeRequestFriend(const string& targetClientUUID);

    void warmup(std::function<void(string userUUID)>& Callback);

    bool isValid() const { return _isValid; }
};