#pragma once
#include "Data/MessagesIncluds.h"
#include "Data/AsioThreads.h"


class Client;
class IAuthClientService;
class IAppVersionService;

class IClientCommandHandler
{

public:
    virtual ~IClientCommandHandler() = default;
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) = 0;
};


class LoginHandler : public IClientCommandHandler {

    //Живет от начала жизни сервера и до конца, поэтому не важно 
    //weak или shared, беру shared для экономии на lock
    shared_ptr<IAuthClientService> _AuthClientService;

public:
    LoginHandler(weak_ptr<IAuthClientService> in_AuthClientService) : _AuthClientService(in_AuthClientService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestFriendHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class ResponseFriendHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestFriendsListHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestAppVersionHandler : public IClientCommandHandler {

    shared_ptr<IAppVersionService> _AppVersionService;

public:
    RequestAppVersionHandler(shared_ptr<IAppVersionService> in_AppVersionService) : _AppVersionService(in_AppVersionService) {};
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestSearchUsersHandler : public IClientCommandHandler {
public:
    virtual asio::awaitable<void> execute(shared_ptr<Client> sender, const JsonObject& data) override;
};

class RequestOpenUsersDataHandler : public IClientCommandHandler {
public:
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


