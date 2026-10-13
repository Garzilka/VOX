#pragma once
#include "../Server.h"
#include "../ServerInterfaces.h"

class MakeServerServiceImpl : public IMakeServerService
{
private:
    weak_ptr<IDBServerService> _DBService;
    weak_ptr<ICacheServerService> _CacheService;

public:
    MakeServerServiceImpl(shared_ptr<IDBServerService> in_DBService, shared_ptr<ICacheServerService> in_CacheService) :
        _DBService(in_DBService), _CacheService(in_CacheService) {}


    virtual void makeServer(const string userUUID, const JsonObject& obj) override
    {
        if (userUUID.empty()) return;

        auto sharedCache = _CacheService.lock();
        if (!sharedCache) return;

        auto sharedDBService = _DBService.lock();
        if (!sharedDBService) return;

        auto ServerState = sharedCache->findOrAddServer(userUUID);

        if (ServerState._State == EServerState::LOADING)
        {
            //TODO: Сделать ответ клиенту: Ошибка, подождите
            return;
        }

        auto Server = ServerState._MakedServer.lock();
        if (!Server)
        {
            //TODO: Сделать ответ клиенту: Неизвестная ошикабка при создании сервера
            return;
        }

        Server->addPendingClients(userUUID);
        sharedDBService->requestMakeServerInDB(std::move(userUUID), obj);
    };
};