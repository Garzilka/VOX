#pragma once

#include "../CommonInterfaces.h"


class ConnectionClientServiceImpl : public IConnectionClientService
{
    shared_ptr<ICacheClientService> cacheService;

public:
    ConnectionClientServiceImpl(shared_ptr<ICacheClientService> in_CacheService) :
        cacheService(in_CacheService) {}

    virtual void handleNewConnection(shared_ptr<Client> client) override;

    virtual void handleClientDisconnected(shared_ptr<Client> client) override;
};