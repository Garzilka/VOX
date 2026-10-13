#include "Client_Dispatcher.h"
#include "Client_Commands/Commands.h"
#include "../Services/Common/CommonInterfaces.h"

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

    //registerHandler(ETypeOfMessage::Message,                    std::make_shared<MessageHandler>());
    //registerHandler(ETypeOfMessage::RequestDirectChats,         std::make_unique<RequestDirectChatsHandler>());
    //registerHandler(ETypeOfMessage::RequestHistoryChannel,      std::make_unique<RequestHistoryChannelHandler>());
    //registerHandler(ETypeOfMessage::OpenOrCreateChat,           std::make_unique<OpenOrCreateChatHandler>());
    //registerHandler(ETypeOfMessage::FriendRequest,              std::make_shared<RequestFriendHandler>());
    //registerHandler(ETypeOfMessage::FriendAccept,               std::make_shared<ResponseFriendHandler>());
    //registerHandler(ETypeOfMessage::FriendsList,                std::make_shared<RequestFriendsListHandler>());
    //registerHandler(ETypeOfMessage::UserPassport,               std::make_shared<RequestOpenUsersDataHandler>());
    //registerHandler(ETypeOfMessage::CreateServer,               std::make_unique<CreateServerHandler>());
    //registerHandler(ETypeOfMessage::RequestServerDetails,       std::make_unique<RequestServerDetailsHandler>());
    //registerHandler(ETypeOfMessage::RequestServerShortData,     std::make_unique<RequestServerShortDataHandler>());
    //registerHandler(ETypeOfMessage::GenerateInvite,             std::make_unique<GenerateInviteHandler>());
    //registerHandler(ETypeOfMessage::AcceptInvite,               std::make_unique<AcceptInviteHandler>());
    //registerHandler(ETypeOfMessage::RequestJoinVoice,           std::make_unique<RequestJoinVoiceHandler>());
    //registerHandler(ETypeOfMessage::RequestLeaveVoice,          std::make_unique<RequestLeaveVoiceHandler>());
    //registerHandler(ETypeOfMessage::SearchUsers,                std::make_shared<RequestSearchUsersHandler>());
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
    }
    co_return;
}
