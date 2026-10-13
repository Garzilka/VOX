#include "ConnectionClientServiceImpl.h"
#include "Client.h"

void ConnectionClientServiceImpl::handleNewConnection(shared_ptr<Client> client)
{
    if (!client) return;
    if (!cacheService) return;
    INFO_CLIENT("Новое подключение: " + client->getSocketDescriptor());

    client->subscribeOnDisconnect([&](const shared_ptr<Client>& c) 
        {
            handleClientDisconnected(c);
        });
    cacheService->addNewClient(client);
    client->start();
}

void ConnectionClientServiceImpl::handleClientDisconnected(const shared_ptr<Client> client)
{
    if (!client) return;
    if (!cacheService) return;

    INFO_CLIENT("Отключился: "  + client->getSocketDescriptor());
    cacheService->removeClient(client);
}
