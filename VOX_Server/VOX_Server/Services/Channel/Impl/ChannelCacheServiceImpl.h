#pragma once

#include "../ChannelInterfaces.h"
#include "../Channel.h"
#include "Data/AsioThreads.h"

class BDTaskQueue;
class INotificationService;

class ChannelCacheServiceImpl : public IChannelCacheService,
    public std::enable_shared_from_this<ChannelCacheServiceImpl>
{
private:
    unordered_map<string, shared_ptr<Channel>> m_readyChannelPassport;
    unordered_map<string, shared_ptr<Channel>> m_wakeupChannelPassport;

    unordered_map<string, shared_ptr<Channel>> m_readyChannelDetails;
    unordered_map<string, shared_ptr<Channel>> m_wakeupChannelDetails;

    shared_ptr<INotificationService> notificationService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    ChannelCacheServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<INotificationService> inNotificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(inNotificationService) {
    }

    virtual bool getChannelPassport(const string& responseClientUUID, const string& channelUUID, SChannelPassport& outPassport) override;
    virtual bool getChannelDetails(const string& responseClientUUID, const string& channelUUID, SChannelDetails& outDetails) override;

    virtual void warmupPassport(std::function<void(string)> successCb, string channelUUID) override;
    virtual void warmupDetails(std::function<void(string)> successCb, string channelUUID) override;

    virtual void syncChannelJoin(const string& channelUUID, const string& userUUID) override;
    virtual void syncChannelLeave(const string& channelUUID, const string& userUUID) override;


private:
    shared_ptr<Channel> findOrAddChannelForPassport(const string& channelUUID);
    shared_ptr<Channel> findOrAddChannelForDetails(const string& channelUUID);

    // Ответы из БД (вызываются публично из обработчика или внутренней лямбды)
    void responsePassportFromDB(const JsonObject& obj);
    void responseDetailsFromDB(const JsonObject& obj);

    void requestPassportInDB(const string& channelUUID);
    void requestDetailsInDB(const string& channelUUID);

    bool loadPassportComplete(const string& channelUUID);
    bool loadDetailsComplete(const string& channelUUID);

    shared_ptr<Channel> takeChannel(std::unordered_map<string, shared_ptr<Channel>>& map, const string& channelUUID);
};
