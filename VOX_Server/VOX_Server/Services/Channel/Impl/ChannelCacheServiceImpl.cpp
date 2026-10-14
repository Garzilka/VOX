#include "ChannelCacheServiceImpl.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"

bool ChannelCacheServiceImpl::getChannelPassport(const string& responseClientUUID, const string& channelUUID, SChannelPassport& outPassport)
{
    if (channelUUID.empty() || responseClientUUID.empty()) return false;

    auto channel = findOrAddChannelForPassport(channelUUID);
    if (!channel) return false;

    return channel->getChannelPassport(outPassport, responseClientUUID);
}

bool ChannelCacheServiceImpl::getChannelDetails(const string& responseClientUUID, const string& channelUUID, SChannelDetails& outDetails)
{
    if (channelUUID.empty() || responseClientUUID.empty()) return false;

    auto channel = findOrAddChannelForDetails(channelUUID);
    if (!channel) return false;

    return channel->getChannelDetails(outDetails, responseClientUUID);
}

shared_ptr<Channel> ChannelCacheServiceImpl::findOrAddChannelForPassport(const string& channelUUID)
{
    if (channelUUID.empty()) return nullptr;
    std::lock_guard<mutex> lock(_Mutex);

    auto ready = m_readyChannelPassport.find(channelUUID);
    if (ready != m_readyChannelPassport.end()) return ready->second;

    auto wakeup = m_wakeupChannelPassport.find(channelUUID);
    if (wakeup != m_wakeupChannelPassport.end()) return wakeup->second;

    auto result = std::make_shared<Channel>();
    m_wakeupChannelPassport.try_emplace(channelUUID, result);

    requestPassportInDB(channelUUID);
    return result;
}

shared_ptr<Channel> ChannelCacheServiceImpl::findOrAddChannelForDetails(const string& channelUUID)
{
    if (channelUUID.empty()) return nullptr;
    std::lock_guard<mutex> lock(_Mutex);

    auto ready = m_readyChannelDetails.find(channelUUID);
    if (ready != m_readyChannelDetails.end()) return ready->second;

    auto wakeup = m_wakeupChannelDetails.find(channelUUID);
    if (wakeup != m_wakeupChannelDetails.end()) return wakeup->second;

    auto result = std::make_shared<Channel>();
    m_wakeupChannelDetails.try_emplace(channelUUID, result);

    requestDetailsInDB(channelUUID);
    return result;
}

void ChannelCacheServiceImpl::requestPassportInDB(const string& channelUUID)
{
    BDTask newTask(EBDTaskType::LoadChannelPassport);
    newTask.targetUuid = channelUUID;

    std::weak_ptr<ChannelCacheServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res]() {
                self->responsePassportFromDB(res);
                });
        }
        };
    m_bdQueue.push(std::move(newTask));
}

void ChannelCacheServiceImpl::requestDetailsInDB(const string& channelUUID)
{
    BDTask newTask(EBDTaskType::LoadChannelDetails);
    newTask.targetUuid = channelUUID;

    std::weak_ptr<ChannelCacheServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res]() {
                self->responseDetailsFromDB(res);
                });
        }
        };
    m_bdQueue.push(std::move(newTask));
}

void ChannelCacheServiceImpl::responsePassportFromDB(const JsonObject& obj)
{
    SChannelPassport serverPas = obj.get<SChannelPassport>();
    shared_ptr<Channel> wakeupChannel = nullptr;

    {
        std::lock_guard<mutex> lock(_Mutex);
        auto it = m_wakeupChannelPassport.find(serverPas.ChatUUID);
        if (it != m_wakeupChannelPassport.end()) wakeupChannel = it->second;
    }

    if (!wakeupChannel) return;

    vector<string> pendingClients;
    if (wakeupChannel->setChannelPassport(serverPas, pendingClients))
    {
        loadPassportComplete(serverPas.ChatUUID);
        if (!pendingClients.empty() && notificationService)
        {
            notificationService->NotifyClients(pendingClients, serverPas);
        }
    }
}

void ChannelCacheServiceImpl::responseDetailsFromDB(const JsonObject& obj)
{
    SChannelDetails serverDetails(obj);
    shared_ptr<Channel> wakeupChannel = nullptr;

    {
        std::lock_guard<mutex> lock(_Mutex);
        auto it = m_wakeupChannelDetails.find(serverDetails.RoomUUID);
        if (it != m_wakeupChannelDetails.end()) wakeupChannel = it->second;
    }

    if (!wakeupChannel) return;

    vector<string> pendingClients;
    if (wakeupChannel->setChannelDetails(serverDetails, pendingClients))
    {
        loadDetailsComplete(serverDetails.RoomUUID);
        if (!pendingClients.empty() && notificationService)
        {
            notificationService->NotifyClients(pendingClients, serverDetails);
        }
    }
}

bool ChannelCacheServiceImpl::loadPassportComplete(const string& channelUUID)
{
    std::lock_guard<mutex> lock(_Mutex);
    auto channel = takeChannel(m_wakeupChannelPassport, channelUUID);
    if (!channel) return false;

    m_readyChannelPassport.try_emplace(channelUUID, channel);
    return true;
}

bool ChannelCacheServiceImpl::loadDetailsComplete(const string& channelUUID)
{
    std::lock_guard<mutex> lock(_Mutex);
    auto channel = takeChannel(m_wakeupChannelDetails, channelUUID);
    if (!channel) return false;

    m_readyChannelDetails.try_emplace(channelUUID, channel);
    return true;
}

shared_ptr<Channel> ChannelCacheServiceImpl::takeChannel(std::unordered_map<string, shared_ptr<Channel>>& map, const string& channelUUID)
{
    auto node = map.extract(channelUUID);
    if (node.empty()) return nullptr;
    return std::move(node.mapped());
}

void ChannelCacheServiceImpl::warmupPassport(std::function<void(string)> successCb, string channelUUID)
{
    auto channel = findOrAddChannelForPassport(channelUUID);
    if (channel) channel->warmupPassport(successCb);
}

void ChannelCacheServiceImpl::warmupDetails(std::function<void(string)> successCb, string channelUUID)
{
    auto channel = findOrAddChannelForDetails(channelUUID);
    if (channel) channel->warmupDetails(successCb);
}

void ChannelCacheServiceImpl::syncChannelJoin(const string& channelUUID, const string& userUUID)
{
    if (channelUUID.empty() || userUUID.empty()) return;

    std::lock_guard<mutex> lock(_Mutex);

    auto readyIt = m_readyChannelPassport.find(channelUUID);
    if (readyIt != m_readyChannelPassport.end())
    {
        readyIt->second->addParticipant(userUUID);
        return;
    }

    auto wakeupIt = m_wakeupChannelPassport.find(channelUUID);
    if (wakeupIt != m_wakeupChannelPassport.end())
    {
        wakeupIt->second->addParticipant(userUUID);
    }
}

void ChannelCacheServiceImpl::syncChannelLeave(const string& channelUUID, const string& userUUID)
{
    if (channelUUID.empty() || userUUID.empty()) return;

    std::lock_guard<mutex> lock(_Mutex);

    auto readyIt = m_readyChannelPassport.find(channelUUID);
    if (readyIt != m_readyChannelPassport.end())
    {
        readyIt->second->removeParticipant(userUUID);
        return;
    }

    auto wakeupIt = m_wakeupChannelPassport.find(channelUUID);
    if (wakeupIt != m_wakeupChannelPassport.end())
    {
        wakeupIt->second->removeParticipant(userUUID);
    }
}
