#pragma once

#include "Data/Common.h"
#include "Data/BDTaskTypes.h"

#include "../Server.h"
#include "../ServerInterfaces.h"

#include "DataBase/BDTaskQueue.h"

class DBServerServiceImpl : public IDBServerService, public std::enable_shared_from_this<DBServerServiceImpl>
{
    weak_ptr <ICacheServerService> _CacheService;
    asio::io_context& _IO_Context;
    BDTaskQueue& _BDQueue;

public:
    DBServerServiceImpl(weak_ptr<ICacheServerService> in_CacheService, asio::io_context& in_IO_Context, BDTaskQueue& in_BDQueue) :
        _CacheService(in_CacheService), _IO_Context(in_IO_Context), _BDQueue(in_BDQueue) {}

    asio::io_context& getIO_Context() { return _IO_Context; }

    virtual void requestMakeServerInDB(const string userUUID, const JsonObject& obj) override
    {
        SCreateServerData serverData = SCreateServerData(obj);
        BDTask newTask(EBDTaskType::CreateServer);
        newTask.payload = SMakeServerTask(serverData.ServerName, std::move(userUUID)).ToJSON();

        newTask.onResult = [sharedDBServerService = shared_from_this()](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (sharedDBServerService)
            {
                auto& ioCtx = sharedDBServerService->getIO_Context();

                asio::post(ioCtx, [sharedDBServerService, res]() mutable
                {
                    try
                    {
                        sharedDBServerService->makeServer(res);
                    }
                    catch (const std::exception& e) { spdlog::error("[Server] Cache update failed: {}", e.what()); }
                });
            }
        };

        _BDQueue.push(std::move(newTask));
    };

private:
    void makeServer(const JsonObject& obj)
    {
        auto SharedCache = _CacheService.lock();
        if (!SharedCache) return;

        SMakeServerResponse MakeServerResponse = SMakeServerResponse::FromJSON(obj);

        auto ServerPTR = SharedCache->findOrAddServer(MakeServerResponse.userUUID)._MakedServer.lock();
        if (!ServerPTR) return;

        SharedCache->setServerToReady(MakeServerResponse._ServerPassport.ServerUUID, MakeServerResponse.userUUID);
        vector<string> ClientsToSend;
        ServerPTR->setServerPassport(MakeServerResponse._ServerPassport, ClientsToSend);

        /*SServersList Message;
        Message.Servers.push_back(MakeServerResponse._ServerPassport);
        _Core.emitNewMessage(ClientsToSend, Message);*/
    }

};