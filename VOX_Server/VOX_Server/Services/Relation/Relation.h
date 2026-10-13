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
    void fillPendingList(vector<string>& result, unordered_set<string>& source);
public:
    explicit Relation() = default;
    bool getUserRelations(SUserRelations& outRelations, const string& clientUUID);

    bool setUserRelations(SUserRelations newUserRelations, vector<string>& result_Pendings);
    void addPendingClients(const string& clientUUID);

    void warmup(std::function<void(string userUUID)>& Callback);

    bool isValid() const { return _isValid; }
};