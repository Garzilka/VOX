#pragma once

/*
class Client;
class Core;



class ChannelsManager
{
    unordered_map<string, shared_ptr<Channel>> m_activeChannels;
    Core& _Core;

public:
    ChannelsManager& operator=(const ChannelsManager&) = delete;
    ChannelsManager(const ChannelsManager&) = delete;
    explicit ChannelsManager(Core& result_Core) :_Core(result_Core) {};

    // Подписка сокета сессии на комнату в памяти
    void subscribeUserToChannel(const string& roomUUID, shared_ptr<Client> client);

    // Очистка сессии из всех комнат при отключении пользователя (для предотвращения утечек)
    void handleUserDisconnect(const string& userUUID);

    // Обработка текстовых пакетов (Message и PrivateMessage)
    void newMessage(shared_ptr<Client> client, const JsonObject& data, const ETypeOfMessage& TypeOfMessage);

    // Ленивая загрузка: получение списка личных чатов (DM и Групп) пользователя
    void handleRequestDirectChats(shared_ptr<Client> client);

    // Универсальный обработчик открытия/создания комнат (Direct, Group, ServerText/Voice)
    void handleOpenOrCreateChat(shared_ptr<Client> client, const JsonObject& data);

    void handleRequestChannelHistory(shared_ptr<Client> client, const JsonObject& data);

    void handleVoiceJoin(shared_ptr<Client> client, const JsonObject& data);
    void handleVoiceLeave(shared_ptr<Client> client, const JsonObject& data);

    static string toBase64Url(const ByteArray& bytes)
    {
        static const char kBase64Chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

        std::string result;
        result.reserve((bytes.size() + 2) / 3 * 4);

        size_t i = 0;
        for (; i + 2 < bytes.size(); i += 3)
        {
            uint32_t triple =
                (static_cast<uint32_t>(bytes[i]) << 16) |
                (static_cast<uint32_t>(bytes[i + 1]) << 8) |
                static_cast<uint32_t>(bytes[i + 2]);

            result.push_back(kBase64Chars[(triple >> 18) & 0x3F]);
            result.push_back(kBase64Chars[(triple >> 12) & 0x3F]);
            result.push_back(kBase64Chars[(triple >> 6) & 0x3F]);
            result.push_back(kBase64Chars[triple & 0x3F]);
        }

        if (i < bytes.size())
        {
            uint32_t triple = 0;
            size_t len = 0;

            if (i < bytes.size())
            {
                triple |= static_cast<uint32_t>(bytes[i]) << 16;
                ++len;
                ++i;
            }
            if (i < bytes.size())
            {
                triple |= static_cast<uint32_t>(bytes[i]) << 8;
                ++len;
                ++i;
            }

            result.push_back(kBase64Chars[(triple >> 18) & 0x3F]);
            result.push_back(kBase64Chars[(triple >> 12) & 0x3F]);

            if (len > 1)
            {
                result.push_back(kBase64Chars[(triple >> 6) & 0x3F]);
            }
        }

        return result;
    }

private:
    void processClientMessage(shared_ptr<Client> client, const SMessageData& msg);
};

void ChannelsManager::subscribeUserToChannel(const string& roomUUID, shared_ptr<Client> client)
{
    if (roomUUID.empty() || !client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    if (!m_activeChannels.contains(roomUUID))
    {
        if (Repository->isChannelExists(roomUUID))
        {
            shared_ptr<Channel> newChannel(new Channel(roomUUID));
            m_activeChannels.insert({ roomUUID, newChannel });
        }
        else
        {
            return;
        }
    }

    m_activeChannels[roomUUID]->subscribe(client->getUUID());
    cout << "CHANNELS MANAGER | Клиент" << client->getUUID() << "успешно подписан на канал:" << roomUUID;
}

void ChannelsManager::handleUserDisconnect(const string& userUUID)
{
    if (userUUID.empty()) return;

    int affectedChannelsCount = 0;
    for (auto it = m_activeChannels.begin(); it != m_activeChannels.end(); ++it)
    {
        const auto& channelPtr = it->second;
        if (channelPtr)
        {
            channelPtr->unsubscribe(userUUID);
        }
    }

    cout << "CHANNELS MANAGER | Клиент" << userUUID
        << "выписан из подписчиков" << affectedChannelsCount << "активных каналов.";
}

void ChannelsManager::newMessage(shared_ptr<Client> client, const JsonObject& data, const ETypeOfMessage& TypeOfMessage)
{
    if (TypeOfMessage == ETypeOfMessage::Message)
    {
        SMessageData message(data);
        processClientMessage(client, message);
    }
}

void ChannelsManager::handleRequestDirectChats(shared_ptr<Client> client)
{
    if (!client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    string myUUID = client->getUUID();

    SRequestDirectChatsData response;
    response.Chats = Repository->getUserDirectChats(myUUID);
    response.Response = true;

    for (const auto& chat : response.Chats)
    {
        subscribeUserToChannel(chat.ChatUUID, client);
    }

    client->sendMessage(response);
    cout << "CHANNELS MANAGER | Пользователю" << myUUID << "лениво отправлено личных чатов:" << response.Chats.size();
}

void ChannelsManager::handleOpenOrCreateChat(shared_ptr<Client> client, const JsonObject& data)
{
    if (!client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    auto ClientManager = _Core.getServerContext()._ClientManager.lock();
    if (!ClientManager) return;

    auto ServerManager = _Core.getServerContext()._ServerManager.lock();
    if (!ServerManager) return;

    SOpenOrCreateChatData inData(data);
    string myUUID = client->getUUID();

    SOpenOrCreateChatData response;
    response.ChatType = inData.ChatType;
    response.TargetUserUUID = inData.TargetUserUUID;
    response.ServerUUID = inData.ServerUUID;
    response.Name = inData.Name;

    if (inData.ChatType == ETypeOfChat::DirectText)
    {
        string roomUUID = Repository->getOrCreatePrivateChannel(myUUID, inData.TargetUserUUID);
        if (!roomUUID.empty())
        {
            response.Response = true;
            response.ChatDetails.ChatUUID = roomUUID;
            response.ChatDetails.ChatType = ETypeOfChat::DirectText;
            response.ChatDetails.ParticipantUUIDs.push_back(myUUID);
            response.ChatDetails.ParticipantUUIDs.push_back(inData.TargetUserUUID);

            subscribeUserToChannel(roomUUID, client);

            shared_ptr<Client> targetClient = ClientManager->findOnlineClient(inData.TargetUserUUID);
            if (targetClient)
            {
                subscribeUserToChannel(roomUUID, targetClient);

                SOpenOrCreateChatData notification = response;
                targetClient->sendMessage(notification);
            }
        }
    }
    else if (inData.ChatType == ETypeOfChat::ServerText || inData.ChatType == ETypeOfChat::ServerVoice)
    {
        if (inData.ServerUUID.empty() || inData.Name.empty()) return;

        string newChannelUUID = generateUuid();

        bool dbSuccess = Repository->createServerChannelInDB(
            newChannelUUID, inData.ServerUUID, inData.Name, inData.ChatType
        );

        if (dbSuccess)
        {
            response.Response = true;
            response.ChatDetails.ChatUUID = newChannelUUID;
            response.ChatDetails.ChatType = inData.ChatType;
            response.ChatDetails.Name = inData.Name;
            response.ChatDetails.ServerUUID = inData.ServerUUID;
            response.ChatDetails.ParentUUID = inData.ParentUUID;

            shared_ptr<Channel> newChannel(new Channel(newChannelUUID));

            m_activeChannels.insert({ newChannelUUID, newChannel });

            auto serverObj = ServerManager->getServer(inData.ServerUUID);
            if (serverObj)
            {
                serverObj.get()->addChannel(newChannelUUID);
                for (const string& mateUUID : serverObj->getOnlineMembers())
                {
                    const shared_ptr<Client>& mate = ClientManager->findOnlineClient(mateUUID);
                    if (!mate)
                    {
                        serverObj->removeOnlineMember(mateUUID);
                        continue;
                    }

                    newChannel->subscribe(mateUUID);
                    mate->sendMessage(response);
                }
            }
            cout << "CHANNELS MANAGER | На сервере" << inData.ServerUUID
                << "успешно создан канал:" << inData.Name << "[" << newChannelUUID << "]";
            return;
        }
    }

    client->sendMessage(response);
}

void ChannelsManager::handleRequestChannelHistory(shared_ptr<Client> client, const JsonObject& data)
{
    if (!client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    SChannelHistoryData inData(data);
    string roomUUID = inData.RoomUUID;

    if (roomUUID.empty()) return;

    SChannelHistoryData response;
    response.Response = false;
    response.RoomUUID = roomUUID;

    if (!m_activeChannels.contains(roomUUID))
    {
        if (Repository->isChannelExists(roomUUID))
        {
            shared_ptr<Channel> newChannel(new Channel(roomUUID));
            m_activeChannels.insert({ roomUUID, newChannel });
        }
    }

    if (m_activeChannels.contains(roomUUID))
    {
        m_activeChannels[roomUUID]->subscribe(client->getUUID());
        response.Messages = m_activeChannels[roomUUID]->GetHistory();
        response.Response = true;
    }

    client->sendMessage(response);

    cout << "CHANNELS MANAGER | Пользователю" << client->getUUID()
        << "отдана ленивая история для комнаты:" << roomUUID
        << " (Сообщений:" << response.Messages.size() << ")";
}

void ChannelsManager::processClientMessage(shared_ptr<Client> client, const SMessageData& msg)
{
    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    string roomUUID = msg.RoomUUID;

    if (m_activeChannels.contains(roomUUID))
    {
        m_activeChannels[roomUUID]->newMessage(msg);
        return;
    }

    if (Repository->isChannelExists(roomUUID))
    {
        shared_ptr<Channel> newChannel(new Channel(roomUUID));
        m_activeChannels.insert({ roomUUID, newChannel } );

        newChannel->subscribe(client->getUUID());
        newChannel->newMessage(msg);
        return;
    }
    cout << "Попытка отправить сообщение в несуществующий канал:" << roomUUID;
}

void ChannelsManager::handleVoiceJoin(shared_ptr<Client> client, const JsonObject& data)
{
    auto repoLock = _Core.getServerContext()._Repository.lock();
    if (!repoLock) return;

    auto serverMgrLock = _Core.getServerContext()._ServerManager.lock();
    if (!serverMgrLock) return;

    auto clientMgrLock = _Core.getServerContext()._ClientManager.lock();
    if (!clientMgrLock) return;

    if (!client) return;

    SVoiceStateData inData(data);
    if (inData.ServerUUID.empty()) return;

    auto serverObj = serverMgrLock->getServer(inData.ServerUUID);
    if (!serverObj) return;

    const std::string& myUUID = client->getUUID();

    if (!repoLock->updateVoiceStateInDB(myUUID, inData.ServerUUID, inData.ChannelUUID)) return;

    const std::string apiKey = "devkey";
    const std::string apiSecret = "secretkey";

    // Header
    JsonObject header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";
    std::string headerJson = header.dump();
    std::vector<unsigned char> headerBytes(headerJson.begin(), headerJson.end());
    std::string encodedHeader = toBase64Url(headerBytes);

    // Payload
    JsonObject payload;
    payload["iss"] = apiKey;
    payload["sub"] = myUUID;
    int64_t expSeconds = currentSecondsSinceEpochPrecise() + (2 * 60 * 60);
    payload["exp"] = expSeconds;

    JsonObject videoGrant;
    videoGrant["roomJoin"] = true;
    videoGrant["room"] = inData.ChannelUUID;
    videoGrant["canPublish"] = true;
    videoGrant["canSubscribe"] = true;

    payload["video"] = videoGrant;
    std::string payloadJson = payload.dump();
    std::vector<unsigned char> payloadBytes(payloadJson.begin(), payloadJson.end());
    std::string encodedPayload = toBase64Url(payloadBytes);

    // Signing input
    std::string signingInput = encodedHeader + "." + encodedPayload;

    // Signature
    std::string encodedSignature = hmacSha256Base64Url(signingInput, apiSecret);

    // Final JWT
    std::string finalJwtToken = encodedHeader + "." + encodedPayload + "." + encodedSignature;

    // Notification
    SVoiceStateData notification(ETypeOfMessage::RequestJoinVoice);
    notification.Response = true;
    notification.ServerUUID = inData.ServerUUID;
    notification.ChannelUUID = inData.ChannelUUID;
    notification.UserUUID = myUUID;
    notification.WebRTCToken = finalJwtToken;

    client->sendMessage(notification);


    const auto& onlineMembers = serverObj->getOnlineMembers();
    for (const auto& mateUUID : onlineMembers)
    {
        auto mate = clientMgrLock->findOnlineClient(mateUUID);
        if (!mate)
        {
            serverObj->removeOnlineMember(mateUUID);
            continue;
        }
        mate->sendMessage(notification);
    }

    std::cout << "VOICE SIGNALING | Сгенерирован JWT-токен LiveKit для пользователя: "
        << myUUID << "\n";
}

void ChannelsManager::handleVoiceLeave(shared_ptr<Client> client, const JsonObject& data)
{
    if (!client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    auto ClientManager = _Core.getServerContext()._ClientManager.lock();
    if (!ClientManager) return;

    auto ServerManager = _Core.getServerContext()._ServerManager.lock();
    if (!ServerManager) return;

    SVoiceStateData inData(data);
    string myUUID = client->getUUID();

    Repository->removeVoiceStateFromDB(myUUID);

    SVoiceStateData notification(ETypeOfMessage::RequestLeaveVoice);
    notification.Response = true;
    notification.ServerUUID = inData.ServerUUID;
    notification.ChannelUUID = inData.ChannelUUID;
    notification.UserUUID = myUUID;

    client->sendMessage(notification);
    if (!inData.ServerUUID.empty())
    {
        auto serverObj = ServerManager->getServer(inData.ServerUUID);
        if (serverObj)
        {
            for (const string& mateUUID : serverObj->getOnlineMembers())
            {
                const shared_ptr<Client>& mate = ClientManager->findOnlineClient(mateUUID);
                if (!mate)
                {
                    serverObj->removeOnlineMember(mateUUID);
                    continue;
                }
                mate->sendMessage(notification);
            }
        }
    }
    cout << "VOICE SIGNALING | Пользователь" << myUUID << "вышел из голосового канала:" << inData.ChannelUUID;
}



*/