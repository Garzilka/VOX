#pragma once
#include "../CommonInterfaces.h"


class NotificationServiceImpl : public INotificationService
{

    shared_ptr<ICacheClientService> cacheService;

public:
    NotificationServiceImpl(shared_ptr<ICacheClientService> in_CacheService) : cacheService(in_CacheService) {}
    virtual void NotifyClients(const vector<string> clietnsUUID, const SBaseMessageData& Message) override;
};