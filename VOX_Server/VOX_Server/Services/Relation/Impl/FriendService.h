#pragma once
#include "../RelationshipInterfaces.h"
#include "Data/Common.h"
#include "Data/AsioThreads.h"


class BDTaskQueue;

class AuthClientServiceImpl : public IFriendService,
    public std::enable_shared_from_this<AuthClientServiceImpl>
{

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    AuthClientServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue) {
    }

    virtual void Friendship(const string& clientUUID, const string& targetClientUUID) override {};
    virtual void FriendAccept(const string& clientUUID, const string& targetClientUUID) override {};
};