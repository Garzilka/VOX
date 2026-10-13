#pragma once
#include "../ServerInterfaces.h"
#include "../Server.h"


class CacheServerServiceImpl : public ICacheServerService 
{
private:
    shared_ptr<IDBServerService> _dbService;
    unordered_map<string, shared_ptr<Server>> m_activeServers;
    unordered_map<string, shared_ptr<Server>> m_awaitMakeServers;
    mutable mutex _Mutex;

public:
    CacheServerServiceImpl(shared_ptr<IDBServerService> db) : _dbService(db) {}

    virtual SGettedServerState findOrAddServer(const string& userUUID) override
    {
        //Главное не заблокировать САМОГО СЕБЯ setServerToReady
        std::lock_guard<mutex> lock(_Mutex);

        if (m_activeServers.contains(userUUID))
        {
            auto it = m_activeServers.find(userUUID);
            if (it != m_activeServers.end())
            {
                SGettedServerState Result = SGettedServerState(EServerState::READY);
                Result._MakedServer = it->second;
                return Result;
            }
        }

        if (m_awaitMakeServers.contains(userUUID))
        {
            auto it = m_awaitMakeServers.find(userUUID);
            if (it != m_awaitMakeServers.end())
            {
                SGettedServerState Result = SGettedServerState(it->second->isValid() ? EServerState::READY : EServerState::LOADING);
                Result._MakedServer = it->second;
                return Result;
            }
        }

        auto [it, inserted] = m_awaitMakeServers.try_emplace(userUUID, std::make_shared<Server>());
        auto server = it->second;

        if (server)
        {
            SGettedServerState Result = SGettedServerState(EServerState::NOT_STARTED);
            Result._MakedServer = server;
            return Result;
        };

        return SGettedServerState(EServerState::FAILED);
    }

    virtual void setServerToReady(const string& newUUID, const string& oldUUID) override
    {
        std::lock_guard<mutex> lock(_Mutex);

        auto it = m_awaitMakeServers.find(oldUUID.empty() ? newUUID : oldUUID);
        if (it == m_awaitMakeServers.end()) return;

        auto ServerPTR = std::move(it->second);
        if (!ServerPTR) return;

        m_awaitMakeServers.erase(it);
        m_activeServers.try_emplace(newUUID, ServerPTR);
    };
};