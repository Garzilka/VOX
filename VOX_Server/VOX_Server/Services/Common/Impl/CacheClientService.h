#pragma once
#include "../CommonInterfaces.h"

class CacheClientServiceImpl : public ICacheClientService
{
    unordered_map<string, shared_ptr<Client>> m_activeClients;
    unordered_map<string, shared_ptr<Client>> m_unauthorizedClients;
    mutable mutex _Mutex;

public:
    CacheClientServiceImpl() {}

    virtual bool addNewClient(shared_ptr<Client> client) override;

    virtual bool authComplete(const string& clientUUID, const string& socketDescriptor) override;

    virtual shared_ptr<Client> findAuthClient(const string& clientUUID) override;

    virtual shared_ptr<Client> findUnauthClient(const string& clientUUID) override;

    virtual void removeClient(shared_ptr<Client> client) override;

private:
    shared_ptr<Client> takeClient(unordered_map<string, shared_ptr<Client>>& map, const string& socketID);
};