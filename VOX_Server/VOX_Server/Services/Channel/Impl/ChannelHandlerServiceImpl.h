#pragma once
#include "../ChannelInterfaces.h"
#include "Data/AsioThreads.h"

class BDTaskQueue;
class INotificationService;

class ChannelHandlerServiceImpl : public IChannelHandlerService,
    public std::enable_shared_from_this<ChannelHandlerServiceImpl>
{
private:
    shared_ptr<INotificationService> notificationService;
    shared_ptr<IChannelCacheService> channelCacheService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

    string m_lkApiKey;
    string m_lkApiSecret;

public:
    ChannelHandlerServiceImpl(asio::io_context& ioCtx,  BDTaskQueue& bdQueue,
        shared_ptr<INotificationService> inNotificationService,
        shared_ptr<IChannelCacheService> inChannelCacheService,
        string lkApiKey,  string lkApiSecret)
        : 
        m_ioContext(ioCtx), m_bdQueue(bdQueue),
        notificationService(inNotificationService),
        channelCacheService(inChannelCacheService),
        m_lkApiKey(std::move(lkApiKey)),
        m_lkApiSecret(std::move(lkApiSecret)) {}

    virtual void handleCreateChannel(const string& responseClientUUID, const SChannelPassport& data) override;
    virtual void handleJoinChannel(const string& responseClientUUID, const SChannelJoin& data) override;
    virtual void handleLeaveChannel(const string& responseClientUUID, const SChannelJoin& data) override;

private:
    void responseCreateFromDB(const JsonObject& obj, const string& clientUUID);
    void responseJoinFromDB(const JsonObject& obj, const string& clientUUID);
    void responseLeaveFromDB(const JsonObject& obj, const string& clientUUID);
};
