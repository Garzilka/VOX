#include "CacheServerService.h"
#include "Data/BDTaskTypes.h"
#include "Data/AsioThreads.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"

bool CacheServerServiceImpl::getServerPassport(const string& userUUID, SServerPassport& Result)
{
    if (userUUID.empty()) return false;

    auto Server = findOrAddServer(Result.ServerUUID);
    if (!Server._MakedServer) return false;

    SServerPassport ServerPas;
    if (!Server._MakedServer->getServerPassport(ServerPas, userUUID)) return false;

    return true;
}

bool CacheServerServiceImpl::getServerDetails(const string& userUUID, SServerDetailsData& Result)
{
    return false;
}

SGettedServerState CacheServerServiceImpl::findOrAddServer(const string& serverUUID)
{
    std::lock_guard<mutex> lock(_Mutex);

    if (serverUUID.empty()) return SGettedServerState(EServerState::FAILED);

    auto it = m_activeServers.find(serverUUID);
    if (it != m_activeServers.end())
    {
        SGettedServerState Result = SGettedServerState(EServerState::READY);
        Result._MakedServer = it->second;
        return Result;
    }

    it = m_awaitMakeServers.find(serverUUID);
    if (it != m_awaitMakeServers.end())
    {
        SGettedServerState Result = SGettedServerState(it->second->isValid() ? EServerState::READY : EServerState::LOADING);
        Result._MakedServer = it->second;
        return Result;
    }
    auto server = std::make_shared<Server>();
    m_awaitMakeServers.try_emplace(serverUUID, server);
    requestPassportInDB(serverUUID);
    if (server)
    {
        SGettedServerState Result = SGettedServerState(EServerState::NOT_STARTED);
        Result._MakedServer = server;
        return Result;
    };

    return SGettedServerState(EServerState::FAILED);
}



void CacheServerServiceImpl::requestPassportInDB(const string serverUUID)
{
    SServerPassport ServerPassport;
    ServerPassport.ServerUUID = serverUUID;

    BDTask newTask(EBDTaskType::LoadServerPassport);

    newTask.payload = ServerPassport.ToJSON();

    auto self = shared_from_this();

    newTask.onResult = [self](bool success, const JsonObject& res)
    {
        if (!success) return;

        if (self)
        {
            auto& ioCtx = self->m_ioContext;

            asio::post(self->m_ioContext, [self, res]()
            {
                self->responsePassportFromDB(res);
            });
        }
    };

    m_bdQueue.push(std::move(newTask));
}

void CacheServerServiceImpl::responsePassportFromDB(const JsonObject& obj)
{
    SServerPassport ServerPassport(obj);
    setServerPassport(ServerPassport);
}


bool CacheServerServiceImpl::setServerPassport(SServerPassport in_Data)
{
    if (in_Data.ServerUUID.empty()) return false;

    vector<string> Users;

    {
        std::lock_guard<mutex> lock(_Mutex);
        auto node = m_awaitMakeServers.extract(in_Data.ServerUUID);
        if (node.empty()) return false;

        auto server = node.mapped();
        if (!server) return false;

        server->setServerPassport(in_Data, Users);

        m_activeServers.try_emplace(in_Data.ServerUUID, std::move(server));
    } 

    if (notificationService && !Users.empty())
    {
        notificationService->NotifyClients(Users, in_Data);
    }

    return true;
}