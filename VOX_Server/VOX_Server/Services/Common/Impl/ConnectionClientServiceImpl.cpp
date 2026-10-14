#include "ConnectionClientServiceImpl.h"
#include "Client.h"

void ConnectionClientServiceImpl::handleNewConnection(shared_ptr<Client> client)
{
    if (!client) return;
    if (!cacheService) return;
    CLIENT_INFO("Новое подключение: " + client->getSocketDescriptor());

    auto cache = cacheService;
    client->subscribeOnDisconnect([cache](const shared_ptr<Client>& c)
        {
            if (c && cache)
            {
                CLIENT_INFO("Отключился: " + c->getSocketDescriptor());
                cache->removeClient(c);
            }
        });

    cacheService->addNewClient(client);
    client->start();
}
