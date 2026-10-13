#pragma once

#include "../../Data/MessagesIncluds.h"

class Client;

class Channel
{

private:
    string m_UUID;
    unordered_set<string> m_activeSubscribers;
    queue<SMessageData> m_historyCache;

public:
    explicit Channel(string UUID);

    void subscribe(const string& _Client_UUID)
    {
        if (_Client_UUID.empty()) return;

        m_activeSubscribers.insert(_Client_UUID);
    }

    void unsubscribe(const string& _Client_UUID)
    {
        if (_Client_UUID.empty()) return;

        m_activeSubscribers.erase(_Client_UUID);
    }

    void newMessage(const SMessageData& msg);

    vector<SMessageData> GetHistory()
    {
        
        return {};// m_historyCache.toList();
    }
};