
#include "commands.h"
#include "Client.h"
#include "../../Services/Common/CommonInterfaces.h"
#include "../../Services/Relation/RelationshipInterfaces.h"
#include "../../Services/Users/UserProfileInterfaces.h"


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

asio::awaitable<void> FriendRequestHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{
    if (!_FriendService) co_return;

    SFriendRequest Req(data);
    Req.SenderUUID = sender->getUUID();
    _FriendService->FriendRequest(Req);

    co_return;
}

asio::awaitable<void> FriendAcceptHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{

    if (!_FriendService) co_return;

    SFriendAccept Req(data);
    Req.SenderUUID = sender->getUUID();
    _FriendService->FriendAccept(Req);

    co_return;
}

asio::awaitable<void> RequestRelationsHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{
    if (!_RelationCacheService) co_return;

    SUserRelations Response;
    if (!_RelationCacheService->handleUserRelations(sender->getUUID(), Response)) co_return;

    sender->sendMessage(Response);
    co_return;
}

asio::awaitable<void> RequestAppVersionHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    if (!_AppVersionService) co_return;

    _AppVersionService->handleAppVersionRequest(sender);
    co_return;
}

asio::awaitable<void> RequestSearchUsersHandler::execute(shared_ptr<Client> sender, const JsonObject& data) 
{
    if (!_SearchUserService) co_return;

    SSearchUsersData Req(data);
    _SearchUserService->handleSearchUsers(sender->getUUID(), Req);
    co_return;
}

asio::awaitable<void> RequestUsersPassportHandler::execute(shared_ptr<Client> sender, const JsonObject& data)
{
    if (!_UserProfileCacheService) co_return;

    SUserPassport Request(data);
    if (!_UserProfileCacheService->getUserPassport(sender->getUUID(), Request)) co_return;

    sender->sendMessage(Request);
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
