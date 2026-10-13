
#include "commands.h"
#include "Client.h"
#include "../../Services/Common/CommonInterfaces.h"



asio::awaitable<void> LoginHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{
    if (!sender) co_return;
    if (!_AuthClientService) co_return;

    SAuthorizationData lData(data);

    if (lData.TypeMessage == ETypeOfMessage::Login)
    {
        _AuthClientService->authClient(sender->getSocketDescriptor(), data);
    }

    if (lData.TypeMessage == ETypeOfMessage::Registration)
    {
        _AuthClientService->regClient(sender->getSocketDescriptor(), data);
    }

    /*if (lData.TypeMessage == ETypeOfMessage::AuthToken)
    {
        _AuthClientService->authViaToken(sender, data);
    }*/
    co_return;
}

asio::awaitable<void> RequestFriendHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{
    co_return;
}

asio::awaitable<void> ResponseFriendHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    co_return;
}

asio::awaitable<void> RequestFriendsListHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    co_return;
}

asio::awaitable<void> RequestAppVersionHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    co_return;
}

asio::awaitable<void> RequestSearchUsersHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    co_return;
}

asio::awaitable<void> RequestOpenUsersDataHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    co_return;
}

/*
void RequestHistoryChannelHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    QChannelsManager::getInstance().handleRequestChannelHistory(sender, data);
}
void MessageHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ETypeOfMessage type = ETypeOfMessage::Message; QChannelsManager::getInstance().newMessage(sender, data, type);
}
void OpenOrCreateChatHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    QChannelsManager::getInstance().handleOpenOrCreateChat(sender, data);
}

void CreateServerHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ServerManager::getInstance().handleCreateServer(sender, data);
}

void RequestServerDetailsHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ServerManager::getInstance().handleRequestServerDetails(sender, data);
}

void RequestServerShortDataHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ServerManager::getInstance().handleRequestServerShortData(sender, data);
}

void GenerateInviteHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ServerManager::getInstance().handleGenerateInvite(sender, data);
}

void AcceptInviteHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    ServerManager::getInstance().handleAcceptInvite(sender, data);
}

void RequestJoinVoiceHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    QChannelsManager::getInstance().handleVoiceJoin(sender, data);
}

void RequestLeaveVoiceHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    QChannelsManager::getInstance().handleVoiceLeave(sender, data);
}




void RequestDirectChatsHandler::execute(shared_ptr<Client> sender, const JsonObject& data) {
    QChannelsManager::getInstance().handleRequestDirectChats(sender);
}*/
