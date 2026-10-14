#pragma once
#include "../../Data/MessagesIncluds.h"


class IRelationCacheService
{
public:
    virtual void warmup(std::function<void(string userUUID)> Success, string userUUID) = 0;
    virtual bool handleUserRelations(const string& responseClientUUID, SUserRelations& outRelations) = 0;
    virtual void addNewFriend(const string& responseClientUUID, const string& targetClientUUID) = 0;
    virtual void addRequestFriend(const string& responseClientUUID, const string& targetClientUUID) = 0;
    virtual void removeFriend(const string& responseClientUUID, const string& targetClientUUID) = 0;
    virtual void removeRequestFriend(const string& responseClientUUID, const string& targetClientUUID) = 0;
};

class IFriendService 
{
public:
    virtual void FriendRequest(const SFriendRequest& requestData) = 0;
    virtual void FriendAccept(const SFriendAccept& requestData) = 0;
};