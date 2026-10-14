#pragma once
#include "Data/MessagesIncluds.h"
#include "Data/AsioThreads.h"


class Client;
class IAuthClientService;
class IAppVersionService;
class IRelationCacheService;
class ISearchUserService;
class IFriendService;
class IUserProfileCacheService;

class IClientCommandHandler
{

public:
    virtual ~IClientCommandHandler() = default;
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) = 0;
};


class LoginHandler : public IClientCommandHandler {

    shared_ptr<IAuthClientService> _AuthClientService;

public:
    LoginHandler(weak_ptr<IAuthClientService> in_AuthClientService) : _AuthClientService(in_AuthClientService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class FriendRequestHandler : public IClientCommandHandler {
    shared_ptr<IFriendService> _FriendService;
public:
    FriendRequestHandler(weak_ptr<IFriendService> in_FriendService) : _FriendService(in_FriendService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class FriendAcceptHandler : public IClientCommandHandler {
    shared_ptr<IFriendService> _FriendService;
public:
    FriendAcceptHandler(weak_ptr<IFriendService> in_FriendService) : _FriendService(in_FriendService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestRelationsHandler : public IClientCommandHandler {
    shared_ptr<IRelationCacheService> _RelationCacheService;
public:
    RequestRelationsHandler(weak_ptr<IRelationCacheService> in_RelationCacheService) : _RelationCacheService(in_RelationCacheService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestAppVersionHandler : public IClientCommandHandler {

    shared_ptr<IAppVersionService> _AppVersionService;

public:
    RequestAppVersionHandler(shared_ptr<IAppVersionService> in_AppVersionService) : _AppVersionService(in_AppVersionService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestSearchUsersHandler : public IClientCommandHandler {
    shared_ptr<ISearchUserService> _SearchUserService;
public:
    RequestSearchUsersHandler(shared_ptr<ISearchUserService> in_SearchUserService) : _SearchUserService(in_SearchUserService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestUsersPassportHandler : public IClientCommandHandler {
    shared_ptr<IUserProfileCacheService> _UserProfileCacheService;
public:
    RequestUsersPassportHandler(shared_ptr<IUserProfileCacheService> in_SearchUserService) : _UserProfileCacheService(in_SearchUserService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

/*class RequestDirectChatsHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};


class MessageHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context, const JsonObject& data) override;
};

class RequestHistoryChannelHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class OpenOrCreateChatHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class CreateServerHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};


class RequestServerShortDataHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class GenerateInviteHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class AcceptInviteHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class RequestJoinVoiceHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};

class RequestLeaveVoiceHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const ServerContext& context) override;
};
*/


