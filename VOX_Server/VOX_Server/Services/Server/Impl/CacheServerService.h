#pragma once
#include "../ServerInterfaces.h"
#include "../Server.h"
#include "Data/AsioThreads.h"

class BDTaskQueue;
class INotificationService;

class CacheServerServiceImpl : public ICacheServerService, 
    public std::enable_shared_from_this<CacheServerServiceImpl>
{
private:
    unordered_map<string, shared_ptr<Server>> m_activeServers;
    unordered_map<string, shared_ptr<Server>> m_awaitMakeServers;
    shared_ptr<INotificationService> notificationService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    CacheServerServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<INotificationService> in_NotificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(in_NotificationService) {
    }

    virtual bool getServerPassport(const string& userUUID, SServerPassport& Result) override;
    virtual bool getServerDetails(const string& userUUID, SServerDetailsData& Result) override;


    virtual bool setServerPassport(SServerPassport in_Data) override;
private:
    virtual SGettedServerState findOrAddServer(const string& userUUID);
    virtual void setServerToReady(const string& newUUID);
    void requestPassportInDB(const string serverUUID);
    void responsePassportFromDB(const JsonObject& obj);
};