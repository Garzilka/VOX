#include "CacheClientService.h"
#include "Client.h"

bool CacheClientServiceImpl::addNewClient(shared_ptr<Client> client)
{
    if (!client) return false;

    string socketDesc = client->getSocketDescriptor();
    string uuid = client->getUUID();

    std::lock_guard<mutex> lock(_Mutex);

    if (!uuid.empty())
    {
        m_activeClients.try_emplace(std::move(uuid), std::move(client));
        return true;
    }

    if (socketDesc.empty()) return false;

    m_unauthorizedClients.try_emplace(std::move(socketDesc), std::move(client));
    return true;
}

bool CacheClientServiceImpl::authComplete(const string& clientUUID, const string& socketDescriptor)
{
    if (clientUUID.empty() || socketDescriptor.empty()) return false;

    std::lock_guard<mutex> lock(_Mutex);

    shared_ptr<Client> authorizedClientPtr = takeClient(m_unauthorizedClients, socketDescriptor);
    if (!authorizedClientPtr) return false;

    m_activeClients.try_emplace(std::move(clientUUID), std::move(authorizedClientPtr));
    return true;
}

shared_ptr<Client> CacheClientServiceImpl::findAuthClient(const string& clientUUID)
{
    if (clientUUID.empty()) return nullptr;

    std::lock_guard<mutex> lock(_Mutex);

    auto it = m_activeClients.find(clientUUID);
    return (it != m_activeClients.end()) ? it->second : nullptr;
}

shared_ptr<Client> CacheClientServiceImpl::findUnauthClient(const string& clientUUID)
{
    if (clientUUID.empty()) return nullptr;

    std::lock_guard<mutex> lock(_Mutex);

    auto it = m_unauthorizedClients.find(clientUUID);
    return (it != m_unauthorizedClients.end()) ? it->second : nullptr;
}

void CacheClientServiceImpl::removeClient(shared_ptr<Client> client)
{
    if (!client) return;

    string socketDesc = client->getSocketDescriptor();
    string uuid = client->getUUID();

    std::lock_guard<mutex> lock(_Mutex);

    if (!uuid.empty())
    {
        m_activeClients.erase(uuid);
    }

    if (!socketDesc.empty())
    {
        m_unauthorizedClients.erase(socketDesc);
    }
}
shared_ptr<Client> CacheClientServiceImpl::takeClient(unordered_map<string, shared_ptr<Client>>& map, const string& socketID)
{
    auto node = map.extract(socketID);
    if (node.empty()) return nullptr;
    return std::move(node.mapped());
}
