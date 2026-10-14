#include "AppVersionService.h"
#include "Client.h"

void AppVersionServiceImpl::handleAppVersionRequest(shared_ptr<Client> client)
{
    if (!client) return;

    std::lock_guard<mutex> lock(m_mutex);
    client->sendMessage(_AppVersion);
}
