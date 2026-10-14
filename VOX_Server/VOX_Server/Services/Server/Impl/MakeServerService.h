#pragma once
#include "../ServerInterfaces.h" 
#include "../Server.h"
#include "Data/AsioThreads.h"

class BDTaskQueue;
class INotificationService;

class ServerServiceImpl : public IServerService,
    public std::enable_shared_from_this<ServerServiceImpl>
{
private:
    shared_ptr<INotificationService> notificationService;
    shared_ptr<ICacheServerService> cacheServerService;
    mutable std::mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    ServerServiceImpl(
        asio::io_context& ioCtx,
        BDTaskQueue& bdQueue,
        std::shared_ptr<INotificationService> in_NotificationService,
        std::shared_ptr<ICacheServerService> in_CacheServerService)
        : m_ioContext(ioCtx),
        m_bdQueue(bdQueue),
        notificationService(in_NotificationService),
        cacheServerService(in_CacheServerService) {
    }

    virtual void makeServer(const string userUUID, const SCreateServerData& Req) override;
    virtual void ServerInvite(const string userUUID, const SGenerateInviteData& Req) override;
    virtual void ServerAccept(const string userUUID, const SAcceptInviteData& obj) override;

private:
    void responseMakeServerFromDB(const JsonObject& obj, const string& clientUUID);
    void responseServerInviteFromDB(const JsonObject& obj, const string& clientUUID);
    void responseServerAcceptFromDB(const JsonObject& obj, const string& clientUUID);
};
