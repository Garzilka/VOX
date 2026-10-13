#pragma once

#include <regex>

#include "../CommonInterfaces.h"
#include "Data/Common.h"
#include "Data/AsioThreads.h"

class BDTaskQueue;

class AuthClientServiceImpl : public IAuthClientService,
    public std::enable_shared_from_this<AuthClientServiceImpl>
{
    shared_ptr<ICacheClientService> _CacheClientService;
    shared_ptr<INotificationService> notificationService;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    AuthClientServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<ICacheClientService> in_CacheService, 
        shared_ptr<INotificationService> in_notificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), _CacheClientService(in_CacheService), notificationService(in_notificationService) {}

    virtual bool authClient(const string& userpeerAddress, const SAuthorizationData& data) override;

    virtual bool regClient(const string& userpeerAddress, const SAuthorizationData& data) override;

    /*virtual bool authViaToken(shared_ptr<Client> client, const SAuthorizationData& data) override
    {
        return false;
    }*/

private:

    void dbResponseAuth(const JsonObject& res);

    bool IsValidInput(const SAuthorizationData& Data, string& outErrorReason);
};