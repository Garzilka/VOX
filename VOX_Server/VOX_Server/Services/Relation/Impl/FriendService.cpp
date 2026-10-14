#include "FriendService.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"
#include "../Relation.h"

void FriendServiceImpl::FriendRequest(const SFriendRequest& requestData)
{
    if (requestData.SenderUUID.empty()) return;
    if (requestData.TargetUUID.empty()) return;

    BDTask newTask(EBDTaskType::FriendRequest);
    newTask.payload = requestData.ToJSON();
    std::weak_ptr<FriendServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
    {
        if (!success) return;

        if (auto self = weakSelf.lock())
        {
            asio::post(self->m_ioContext, [self, res]()
                {
                    self->responseFriendRequestFromDB(res);
                });
        }
    };

    m_bdQueue.push(std::move(newTask));
}

void FriendServiceImpl::FriendAccept(const SFriendAccept& requestData)
{
    if (requestData.SenderUUID.empty()) return;
    if (requestData.TargetUUID.empty()) return;

    BDTask newTask(EBDTaskType::FriendAccept);
    newTask.payload = requestData.ToJSON();
    std::weak_ptr<FriendServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
    {
        if (!success) return;

        if (auto self = weakSelf.lock())
        {
            asio::post(self->m_ioContext, [self, res]()
            {
                self->responseFriendAcceptFromDB(res);
            });
        }
    };

    m_bdQueue.push(std::move(newTask));
}

void FriendServiceImpl::responseFriendRequestFromDB(const JsonObject& obj)
{
    SFriendRequest Req(obj);
    if (!relationCacheService) return;
    relationCacheService->addRequestFriend(Req.SenderUUID, Req.TargetUUID);

    if (!notificationService) return;

    SUserRelations Response;
    if (!relationCacheService->handleUserRelations(Req.SenderUUID, Response)) return;

    notificationService->NotifyClients({ Req.SenderUUID }, Response);
}

void FriendServiceImpl::responseFriendAcceptFromDB(const JsonObject& obj)
{
    SFriendAccept Req(obj);
    if (!relationCacheService) return;

    if (Req.Action)
    {
        relationCacheService->addNewFriend(Req.SenderUUID, Req.TargetUUID);
    }
    else
    {
        relationCacheService->removeRequestFriend(Req.SenderUUID, Req.TargetUUID);
    }
    if (!relationCacheService) return;

    SUserRelations Response;
    if (!relationCacheService->handleUserRelations(Req.SenderUUID, Response)) return;

    notificationService->NotifyClients({ Req.SenderUUID }, Response);
}
