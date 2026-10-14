#pragma once
#include "../../Data/MessagesIncluds.h"

class IChannelCacheService
{
public:
    virtual ~IChannelCacheService() = default;
    virtual bool getChannelPassport(const string& responseClientUUID, const string& channelUUID, SChannelPassport& outPassport) = 0;
    virtual bool getChannelDetails(const string& responseClientUUID, const string& channelUUID, SChannelDetails& outDetails) = 0;
    virtual void warmupPassport(std::function<void(string channelUUID)> successCb, string channelUUID) = 0;
    virtual void warmupDetails(std::function<void(string channelUUID)> successCb, string channelUUID) = 0;

    virtual void syncChannelJoin(const string& channelUUID, const string& userUUID) = 0;
    virtual void syncChannelLeave(const string& channelUUID, const string& userUUID) = 0;

};

class IChannelHandlerService
{
public:
    virtual ~IChannelHandlerService() = default;
    virtual void handleCreateChannel(const string& responseClientUUID, const SChannelPassport& data) = 0;
    virtual void handleJoinChannel(const string& responseClientUUID, const SChannelJoin& data) = 0;
    virtual void handleLeaveChannel(const string& responseClientUUID, const SChannelJoin& data) = 0;
};