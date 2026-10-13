#pragma once

#include "Data/Common.h"
#include "Dispatch/Client_Dispatcher.h"
#include "Dispatch/BD_Dispatcher.h"
#include "DataBase/BDWorkerPool.h"


class Core 
{

    ContainerContext m_ctx;


    shared_ptr<Repository> _Repository;
    shared_ptr<BD_Dispatcher> _BD_Dispatcher;
    BDTaskQueue _BDQueue;
    shared_ptr<BDWorkerPool> _BDWorkerPool;

    asio::awaitable<void> listenLoop(uint16_t port);
    asio::io_context m_ioContext;
    bool m_isRunning;
public:
    Core();
    ~Core();
    asio::io_context& getIoContext() { return m_ioContext; }
    bool startServer(uint16_t port);
    void stopServer();
    shared_ptr<ClientMessageDispatcher> _ClientDispatcher;
};

