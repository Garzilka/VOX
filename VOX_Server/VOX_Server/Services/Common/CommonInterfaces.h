#pragma once

#include "Client.h"
#include "Data/Common.h"
#include "Data/MessagesIncluds.h"

class INotificationService {
public:
    virtual void NotifyClients(const vector<string> clietnsUUID, const SBaseMessageData Message) = 0;
};

class IConnectionClientService {
public:
    virtual void handleNewConnection(shared_ptr<Client> client) = 0;
    virtual void handleClientDisconnected(const shared_ptr<Client> client) = 0;
};

class ICacheClientService {
public:
    virtual bool addNewClient(shared_ptr<Client> client) = 0;
    virtual void removeClient(shared_ptr<Client> client) = 0;
    virtual shared_ptr<Client> findAuthClient(const string& clientUUID) = 0;
    virtual shared_ptr<Client> findUnauthClient(const string& clientUUID) = 0;
    virtual bool authComplete(const string& clientUUID, const string& socketDescriptor) = 0;
};

class IAuthClientService {
public:
    virtual bool authClient(const string& userpeerAddress, const SAuthorizationData& data) = 0;
    virtual bool regClient(const string& userpeerAddress, const SAuthorizationData& data) = 0;
    //virtual bool authViaToken(const SAuthorizationData& data) = 0;
};

class IAppVersionService {
public:
    virtual void handleAppVersionRequest(shared_ptr<Client> client, const JsonObject& data) = 0;
};

class IInviteService {
public:
    //virtual void InviteServer(const string& clientUUID, const string& serverUUID) = 0;
};




