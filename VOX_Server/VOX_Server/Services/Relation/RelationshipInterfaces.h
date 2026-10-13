#pragma once
#include "../../Data/MessagesIncluds.h"

class IFriendCacheService {
public:

    bool GetFriendsList(const string& clientUUID, unordered_set<string>& Result);
    unordered_set<string> getBannedList(const string& clientUUID, unordered_set<string>& Result);
    unordered_map<string, SPeerCustomSettings> getPeerSettings(const string& clientUUID, unordered_map<string, SPeerCustomSettings>& Result);
    SUserRelations getUserRelations(const const string& clientUUID, SUserRelations& Result);
    void warmup(std::function<void(string userUUID)> Success);
};


class IFriendService 
{
public:
    virtual void Friendship(const string& clientUUID, const string& targetClientUUID) = 0;
    virtual void FriendAccept(const string& clientUUID, const string& targetClientUUID) = 0;
};