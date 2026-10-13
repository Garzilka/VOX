#pragma once
#include "../RelationshipInterfaces.h"
#include "Data/Common.h"
#include "Data/AsioThreads.h"


class BDTaskQueue;

class AuthClientServiceImpl : public IFriendCacheService,
    public std::enable_shared_from_this<AuthClientServiceImpl>
{

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    AuthClientServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue) {
    }


    void GetFriendsList(const string& clientUUID) override { return false; };
    unordered_set<string> getBannedList(const string& clientUUID);
    unordered_map<string, SPeerCustomSettings> getPeerSettings(const string& clientUUID);
    SUserRelations getUserRelations(const string& clientUUID);
};