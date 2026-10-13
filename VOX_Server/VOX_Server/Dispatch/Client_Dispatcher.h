#pragma once
#include "Data/MessagesIncluds.h"

#include "Libs/DI/DIServiceContainer.h"
#include "../Services/Common/Impl/AuthClientService.h"

class Client;
class IClientCommandHandler;

struct ContainerContext
{
    shared_ptr<DIServiceContainer> CommonServices;
    shared_ptr<DIServiceContainer> ChannelServices;
    shared_ptr<DIServiceContainer> ServerServices;
    shared_ptr<DIServiceContainer> UsersServices;
    shared_ptr<DIServiceContainer> RelationServices;
    shared_ptr<DIServiceContainer> CacheServices;
};


class ClientMessageDispatcher
{
    ContainerContext m_ctx;
    unordered_map<ETypeOfMessage, shared_ptr<IClientCommandHandler>> m_Commands;


public:
    explicit ClientMessageDispatcher(const ContainerContext& in_ctx);

    void registerHandler(ETypeOfMessage type, shared_ptr<IClientCommandHandler> handler)
    {
        m_Commands.insert({ type, handler });
    }

    asio::awaitable<void> dispatch(ETypeOfMessage type, const JsonObject& data, shared_ptr<Client> sender);
};