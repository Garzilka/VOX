#include "Core.h"
#include "Client.h"
#include "Services/Common/Impl/AppVersionService.h"
#include "Services/Common/Impl/AuthClientService.h"
#include "Services/Common/Impl/CacheClientService.h"
#include "Services/Common/Impl/ConnectionClientServiceImpl.h"
#include "Services/Common/Impl/NotificationServiceImpl.h"


Core::Core() : m_isRunning(false)
{
    _Repository = std::make_shared<Repository>();
    _Repository->Run();

    m_ctx.CommonServices = std::make_shared<DIServiceContainer>();
    m_ctx.ChannelServices = std::make_shared<DIServiceContainer>();
    m_ctx.ServerServices = std::make_shared<DIServiceContainer>();
    m_ctx.UsersServices = std::make_shared<DIServiceContainer>();
    m_ctx.RelationServices = std::make_shared<DIServiceContainer>();
    m_ctx.CacheServices = std::make_shared<DIServiceContainer>();

    //====================================
    // Cache
    //====================================
    auto CacheClientService = std::make_shared<CacheClientServiceImpl>();
    m_ctx.CacheServices->registerInstance<ICacheClientService>(CacheClientService);

    //====================================
    // Common
    //====================================

    auto NotificationService = std::make_shared<NotificationServiceImpl>(CacheClientService);
    m_ctx.CommonServices->registerInstance<INotificationService>(NotificationService);

    auto AppVersionService = std::make_shared<AppVersionServiceImpl>(m_ioContext, _BDQueue);
    m_ctx.CommonServices->registerInstance<IAppVersionService>(AppVersionService);

    auto AuthClientService = std::make_shared<AuthClientServiceImpl>(m_ioContext, _BDQueue, CacheClientService, NotificationService);
    m_ctx.CommonServices->registerInstance<IAuthClientService>(AuthClientService);

    auto ConnectionClientService = std::make_shared<ConnectionClientServiceImpl>(CacheClientService);
    m_ctx.CommonServices->registerInstance<IConnectionClientService>(ConnectionClientService);



    _ClientDispatcher = std::make_shared<ClientMessageDispatcher>(m_ctx);
    _BD_Dispatcher = std::make_shared<BD_Dispatcher>(_Repository);
    _BDWorkerPool = std::make_shared<BDWorkerPool>(1, _BDQueue, _BD_Dispatcher);
}

Core::~Core() { stopServer(); }

bool Core::startServer(uint16_t port)
{
    try 
    {
        m_isRunning = true;
        asio::co_spawn(m_ioContext, listenLoop(port), asio::detached);

        cout << "[Core] VOX Server запущен (задачи добавлены в очередь).\n";

        return true;
    }
    catch (const std::exception& e) 
    {
        cerr << "[Core] Ошибка при старте: " << e.what() << "\n";
        return false;
    }
}

void Core::stopServer() 
{
    if (!m_isRunning) return;
    m_isRunning = false;

    m_ioContext.stop();
    cout << "[Core] Сервер остановлен (остановка запланирована).\n";
}

asio::awaitable<void> Core::listenLoop(uint16_t port) 
{
    auto executor = co_await asio::this_coro::executor;
    asio::ip::tcp::acceptor acceptor(executor, { asio::ip::tcp::v4(), port });
    acceptor.set_option(asio::socket_base::reuse_address(true));

    for (;;) 
    {
        // Если stopServer() вызвал stop(), эта строка выбросит исключение operation_aborted
        try 
        {
            asio::ip::tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
            cout << "[Core] Принято подключение: " << socket.remote_endpoint().address().to_string() << "\n";

            auto newClient = std::make_shared<Client>(std::move(socket), *this);
            auto ConnectionService = m_ctx.CommonServices->resolve<IConnectionClientService>();
            if (!ConnectionService) co_return;

            ConnectionService->handleNewConnection(newClient);
        }
        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                cout << "[Core] Цикл прослушивания остановлен.\n";
                break;
            }
            throw;
        }
    }
}