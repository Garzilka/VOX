#include "MakeServerService.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"


void ServerServiceImpl::makeServer(const string userUUID, const SCreateServerData& Req)
{
    if (userUUID.empty() || Req.ServerName.empty()) return;

    BDTask newTask(EBDTaskType::CreateServer); 
    newTask.payload = Req.ToJSON();

    std::weak_ptr<ServerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, userUUID](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) 
        {
            asio::post(self->m_ioContext, [self, res, userUUID]() 
            {
                self->responseMakeServerFromDB(res, userUUID);
            });
        }
        };

    m_bdQueue.push(std::move(newTask));
}

void ServerServiceImpl::responseMakeServerFromDB(const JsonObject& obj, const string& clientUUID)
{
    SServerPassport createdPassport = SServerPassport::FromJSON(obj);
    if (createdPassport.ServerUUID.empty()) return;

    if (cacheServerService)
    {
        cacheServerService->setServerPassport(createdPassport);
    }

    if (notificationService)
    {
        notificationService->NotifyClients({ clientUUID }, createdPassport);
    }
}

void ServerServiceImpl::ServerInvite(const string userUUID, const SGenerateInviteData& Req)
{
    if (userUUID.empty() || Req.ServerUUID.empty()) return;

    BDTask newTask(EBDTaskType::ServerInvite);

    newTask.payload = Req.ToJSON();

    std::weak_ptr<ServerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, userUUID](bool success, const JsonObject& res) 
    {
        if (!success) return;

        if (auto self = weakSelf.lock()) 
        {
            asio::post(self->m_ioContext, [self, res, userUUID]() 
            {
                self->responseServerInviteFromDB(res, userUUID);
            });
        }
    };

    m_bdQueue.push(std::move(newTask));
}

void ServerServiceImpl::responseServerInviteFromDB(const JsonObject& obj, const string& clientUUID)
{
    SGenerateInviteData responseInvite(obj);
    if (responseInvite.ServerUUID.empty()) return;

    if (notificationService)
    {
        notificationService->NotifyClients({ clientUUID }, responseInvite);
    }
}

void ServerServiceImpl::ServerAccept(const string userUUID, const SAcceptInviteData& obj)
{
    if (userUUID.empty() || obj.Token.empty()) return;

    BDTask newTask(EBDTaskType::ServerAccept);
    newTask.payload = obj.ToJSON();

    std::weak_ptr<ServerServiceImpl> weakSelf = shared_from_this();
    newTask.onResult = [weakSelf, userUUID](bool success, const JsonObject& res) {
        if (!success) return;
        if (auto self = weakSelf.lock()) {
            asio::post(self->m_ioContext, [self, res, userUUID]() {
                self->responseServerAcceptFromDB(res, userUUID);
                });
        }
        };

    m_bdQueue.push(std::move(newTask));
}

void ServerServiceImpl::responseServerAcceptFromDB(const JsonObject& obj, const string& clientUUID)
{
    SAcceptInviteData responseAccept(obj);
    if (responseAccept.ServerUUID.empty()) return;


    if (notificationService)
    {
        notificationService->NotifyClients({ clientUUID }, responseAccept);
    }
}
