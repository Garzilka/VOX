#include "Client_Dispatcher.h"
#include "Client_Commands/Commands.h"
#include "../Services/Common/CommonInterfaces.h"
#include "../Services/Relation/RelationshipInterfaces.h"
#include "../Services/Users/UserProfileInterfaces.h"

ClientMessageDispatcher::ClientMessageDispatcher(const ContainerContext& in_ctx) : m_ctx(in_ctx)
{
    if (m_ctx.CommonServices)
    {
        if (auto Service = m_ctx.CommonServices->resolve<IAuthClientService>())
        {
            registerHandler(ETypeOfMessage::Login,          std::make_shared<LoginHandler>(Service));
            registerHandler(ETypeOfMessage::Registration,   std::make_shared<LoginHandler>(Service));
            registerHandler(ETypeOfMessage::AuthToken,      std::make_shared<LoginHandler>(Service));
        }
        if (auto Service = m_ctx.CommonServices->resolve<IAppVersionService>())
        {
            registerHandler(ETypeOfMessage::AppVersion,     std::make_shared<RequestAppVersionHandler>(Service));
        }
    }
    if (m_ctx.CacheServices)
    {
        if (auto Service = m_ctx.CacheServices->resolve<IRelationCacheService>())
        {
            registerHandler(ETypeOfMessage::Relations, std::make_shared<RequestRelationsHandler>(Service));
        }
        if (auto Service = m_ctx.RelationServices->resolve<IFriendService>())
        {
            registerHandler(ETypeOfMessage::FriendRequest, std::make_unique<FriendRequestHandler>(Service));
            registerHandler(ETypeOfMessage::FriendAccept, std::make_unique<FriendAcceptHandler>(Service));
        }
        if (auto Service = m_ctx.CacheServices->resolve<IUserProfileCacheService>())
        {
            registerHandler(ETypeOfMessage::UserPassport, std::make_unique<RequestUsersPassportHandler>(Service));
        }
    }
    if (m_ctx.UsersServices)
    {
        if (auto Service = m_ctx.UsersServices->resolve<ISearchUserService>())
        {
            registerHandler(ETypeOfMessage::SearchUsers, std::make_unique<RequestSearchUsersHandler>(Service));
        }
    }
}

asio::awaitable<void> ClientMessageDispatcher::dispatch(ETypeOfMessage type, const JsonObject& data, shared_ptr<Client> sender)
{
    auto it = m_Commands.find(type);
    if (it != m_Commands.end())
    {
        co_await it->second->execute(sender, data);
    }
    else
    {
        cerr << "[Dispatcher] Неизвестный тип сообщения: " << static_cast<int>(type) << "\n";
        cerr << endl;
    }
    co_return;
}
