#include "Channel.h"

Channel::Channel(string UUID) : m_UUID(UUID)
{
    vector<SMessageData> dbMessages;// = Repository::getInstance().loadHistoryFromDB(m_UUID, 50, 0);

    for (const SMessageData& msg : dbMessages)
    {
        m_historyCache.push(msg);
    }
}

void Channel::newMessage(const SMessageData& msg)
{
    m_historyCache.push(msg);
    if (m_historyCache.size() > 100)
    {
        m_historyCache.front();
        m_historyCache.pop();
    }

    /*for (auto it = m_activeSubscribers.begin(); it != m_activeSubscribers.end(); )
    {
        if (shared_ptr<Client> strongClient = QClientManager::getInstance().findOnlineClient(it.key()))
        {
            strongClient->sendMessage(msg);
            ++it;
        }
        else
        {
            it = m_activeSubscribers.erase(it); // Авто-выписка отключившихся
        }
    }

    Repository::getInstance().saveMessage(msg);*/
}