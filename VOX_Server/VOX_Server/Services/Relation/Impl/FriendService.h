#pragma once
#include "../RelationshipInterfaces.h"
#include "Data/Common.h"
#include "Data/AsioThreads.h"


class BDTaskQueue;
class INotificationService;

class FriendServiceImpl : public IFriendService,
    public std::enable_shared_from_this<FriendServiceImpl>
{

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;
    shared_ptr<INotificationService> notificationService;
    shared_ptr<IRelationCacheService> relationCacheService;

public:
    FriendServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, 
        shared_ptr<INotificationService> in_notificationService, shared_ptr<IRelationCacheService> in_RelationCacheService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(in_notificationService), relationCacheService(in_RelationCacheService) {
    }

    virtual void FriendRequest(const SFriendRequest& requestData) override;
    virtual void FriendAccept(const SFriendAccept& requestData) override;

private:
    void responseFriendRequestFromDB(const JsonObject& obj);
    void responseFriendAcceptFromDB(const JsonObject& obj);
};