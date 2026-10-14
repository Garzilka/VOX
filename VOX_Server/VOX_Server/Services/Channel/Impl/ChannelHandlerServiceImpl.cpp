#include "ChannelHandlerServiceImpl.h"

#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"

void ChannelHandlerServiceImpl::handleCreateChannel(const string& responseClientUUID, const SChannelPassport& data)
{
    if (responseClientUUID.empty()) return;

    BDTask newTask(EBDTaskType::ChannelCreate);
    newTask.payload = data.ToJSON();

    std::weak_ptr<ChannelHandlerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, responseClientUUID](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res, responseClientUUID]() {
                self->responseCreateFromDB(res, responseClientUUID);
                });
        }
        };

    m_bdQueue.push(std::move(newTask));
}

void ChannelHandlerServiceImpl::handleJoinChannel(const string& responseClientUUID, const SChannelJoin& data)
{
    if (responseClientUUID.empty() || data.ChannelUUID.empty()) return;

    BDTask newTask(EBDTaskType::ChannelJoin);
    newTask.payload = data.ToJSON();

    std::weak_ptr<ChannelHandlerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, responseClientUUID](bool success, const JsonObject& res) 
        {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res, responseClientUUID]() 
                {
                self->responseJoinFromDB(res, responseClientUUID);
                });
        }
        };

    m_bdQueue.push(std::move(newTask));
}

void ChannelHandlerServiceImpl::handleLeaveChannel(const string& responseClientUUID, const SChannelJoin& data)
{
    if (responseClientUUID.empty() || data.ChannelUUID.empty()) return;

    BDTask newTask(EBDTaskType::ChannelLeave);
    newTask.payload = data.ToJSON();

    std::weak_ptr<ChannelHandlerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, responseClientUUID](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res, responseClientUUID]() {
                self->responseLeaveFromDB(res, responseClientUUID);
                });
        }
        };

    m_bdQueue.push(std::move(newTask));
}

void ChannelHandlerServiceImpl::responseCreateFromDB(const JsonObject& obj, const string& clientUUID)
{
    SChannelPassport responseData(obj);
    if (responseData.ChatUUID.empty()) return;

    notificationService->NotifyClients({ clientUUID }, responseData);
}

void ChannelHandlerServiceImpl::responseJoinFromDB(const JsonObject& obj, const string& clientUUID)
{
    SChannelJoin responseData(obj);
    if (responseData.ChannelUUID.empty()) return;

    // ГЕНЕРАЦИЯ: Передаем apiKey и apiSecret из контекста класса
    // Здесь также можно добавить проверку, что тип канала является голосовым/видео
    responseData.WebRTCToken = ToolLib::generateJWT(m_lkApiKey, m_lkApiSecret, responseData.UserUUID, responseData.ChannelUUID);

    // СИНХРОНИЗАЦИЯ: Добавляем пользователя в кэш паспорта канала
    if (channelCacheService)
    {
        channelCacheService->syncChannelJoin(responseData.ChannelUUID, responseData.UserUUID);
    }

    // Уведомляем пользователя о том, что он успешно вошел
    notificationService->NotifyClients({ clientUUID }, responseData);
}

void ChannelHandlerServiceImpl::responseLeaveFromDB(const JsonObject& obj, const string& clientUUID)
{
    SChannelJoin responseData(obj);
    if (responseData.ChannelUUID.empty()) return;

    if (channelCacheService)
    {
        channelCacheService->syncChannelLeave(responseData.ChannelUUID, responseData.UserUUID);
    }

    notificationService->NotifyClients({ clientUUID }, responseData);
}
