#pragma once
/*

class ServerManager 
{
    // Кэш серверов в оперативной памяти сервера
    unordered_map<string, shared_ptr<Server>> m_activeServers;
    unordered_map<string, shared_ptr<Server>> m_awaitMakeServers;
    unordered_set<string> _AwaitedDetails;

    mutable mutex _Mutex;

    Core& _Core;
    BDTaskQueue& _BDQueue;
public:
    ServerManager& operator=(const ServerManager&) = delete;
    ServerManager(const ServerManager&) = delete;
    Core& getCore() { return _Core; };
    explicit ServerManager(Core& result_Core, BDTaskQueue& result_BDQueue) : _Core(result_Core), _BDQueue(result_BDQueue) {};

    void handleCreateServer(const string& clientUUID, const JsonObject& data);

    void handleRequestServerShortData(const string& clientUUID, const JsonObject& data);
    void handleRequestServerDetails(const string& clientUUID, const JsonObject& data);

    void handleGenerateInvite(const string& clientUUID, const JsonObject& data);
    void handleAcceptInvite(const string& clientUUID, const JsonObject& data);

private:
    SGettedServerState findOrAddServer(const string& serverName);
    void setServerToReady(const string& serverName, const string beforeUUID);

    void makeServer(const JsonObject& obj);

    void requestPassportInDB(const string& serverUUID);
    void responsePassportFromDB(const JsonObject& obj);

    void requestDetailsInDB(const string& serverUUID);
    void responseDetailsFromDB(const JsonObject& obj);
};
void ServerManager::handleRequestServerShortData(const string& clientUUID, const JsonObject& data)
{
    if (clientUUID.empty()) return;

    SServersList Message(data);

    if (Message.UUIDs.empty())
    {
        cout << "SERVER MANAGER | Получен пустой запрос RequestServerShortData от сессии:" << clientUUID;
        return;
    }

    Message.TypeMessage = ETypeOfMessage::RequestServerShortData;
    Message.Response = true;
    Message.Servers.clear();

    for (const auto& serverUUID : Message.UUIDs)
    {
        auto Server = findOrAddServer(serverUUID)._MakedServer.lock();
        if (!Server) continue;

        SServerPassport ServerPas;
        if (!Server->getServerPassport(ServerPas, clientUUID)) continue;

        Message.Servers.push_back(ServerPas);
    }

    cout << "SERVER MANAGER | Отправлен пакет паспортов серверов (Успешно найдено:"
        << Message.Servers.size() << ") для юзера:" << clientUUID;

    _Core.emitNewMessage({ clientUUID }, Message);
}

void ServerManager::requestPassportInDB(const string& serverUUID)
{
    auto weakServer = _Core.getServerContext()._ServerManager;

    BDTask newTask(EBDTaskType::LoadPassport);
    newTask.payload = SRequestByServerTask(serverUUID).ToJSON();

    newTask.onResult = [serverPtr = std::move(weakServer)](bool success, const JsonObject& res)
    {
        if (!success) return;

        if (auto sharedServer = serverPtr.lock())
        {
            auto& ioCtx = sharedServer->getCore().getIoContext();
            asio::post(ioCtx, [sharedServer, res]() mutable
            {
                try
                {
                    sharedServer->responsePassportFromDB(res);
                }
                catch (const std::exception& e) { spdlog::error("[Server] Cache update failed: {}", e.what()); }
            });
        }
    };

    _BDQueue.push(std::move(newTask));
}

void ServerManager::responsePassportFromDB(const JsonObject& obj)
{
    SServerPassport passport = SServerPassport::FromJSON(obj);
    auto Server = findOrAddServer(passport.ServerUUID)._MakedServer.lock();
    if (!Server) return;

    vector<string> users;
    Server->setServerPassport(std::move(passport), users);

    setServerToReady(passport.ServerUUID, "");

    if (Server->isDetailsNeedLoaded())
    {
        requestDetailsInDB(passport.ServerUUID);
    }

    SServersList Message;
    Message.Servers.push_back(passport);
    _Core.emitNewMessage(users, Message);
}

void ServerManager::handleRequestServerDetails(const string& clientUUID, const JsonObject& data)
{
    if (clientUUID.empty()) return;


    SServerDetailsData Message(data);

    if (Message.ServerUUID.empty()) return;

    auto ServerData = findOrAddServer(Message.ServerUUID);

    auto Server = ServerData._MakedServer.lock();
    if (!Server)
    {
        //TODO: Сделать ответ клиенту: Неизвестная ошикабка при загрузке информации о сервере
        return;
    }

    if (!Server->getServerDetails(Message, clientUUID))
    {
        requestDetailsInDB(Message.ServerUUID);
        return;
    }

    _Core.emitNewMessage({ clientUUID }, Message);
}


void ServerManager::requestDetailsInDB(const string& serverUUID)
{
    {
        std::lock_guard<mutex> lock(_Mutex);
        if (_AwaitedDetails.contains(serverUUID)) return;
        _AwaitedDetails.insert(serverUUID);
    }

    auto weakServer = _Core.getServerContext()._ServerManager;

    BDTask newTask(EBDTaskType::LoadDetails);
    newTask.payload = SRequestByServerTask(serverUUID).ToJSON();

    newTask.onResult = [serverPtr = std::move(weakServer)](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto sharedServer = serverPtr.lock())
            {
                auto& ioCtx = sharedServer->getCore().getIoContext();
                asio::post(ioCtx, [sharedServer, res]() mutable
                    {
                        try
                        {
                            sharedServer->responsePassportFromDB(res);
                        }
                        catch (const std::exception& e) { spdlog::error("[Server] Cache update failed: {}", e.what()); }
                    });
            }
        };

    _BDQueue.push(std::move(newTask));
}

void ServerManager::responseDetailsFromDB(const JsonObject& obj)
{
    SServerDetailsData Details = SServerDetailsData(obj);
    auto Server = findOrAddServer(Details.ServerUUID)._MakedServer.lock();
    if (!Server) return;

    vector<string> users;
    Server->setServerDetails(Details, users);
    _Core.emitNewMessage(users, Details);
}

void ServerManager::handleGenerateInvite(const string& clientUUID, const JsonObject& data)
{
    if (clientUUID.empty()) return;


    SGenerateInviteData inData(data);
    if (inData.ServerUUID.empty() || inData.TargetUserUUID.empty()) return;

    string dmRoomUUID = Repository->getOrCreatePrivateChannel(clientUUID, inData.TargetUserUUID);
    SGenerateInviteData response(inData.ServerUUID, inData.TargetUserUUID);
    response.Response = false;

    if (dmRoomUUID.empty())
    {
        cout << "SERVER MANAGER | Не удалось найти/создать ЛС-комнату для отправки инвайта!";
        client->sendMessage(response);
        return;
    }

    string token = generateUuid();

    if (!Repository->createInviteInDB(token, inData.ServerUUID, clientUUID))
    {
        cout << "SERVER MANAGER | Не удалось создать инвайт!";
        client->sendMessage(response);
        return;
    }

    cout << "SERVER MANAGER | Создан инвайт-токен:" << token << "для сервера:" << inData.ServerUUID;

    SAttachmentData inviteAttachment;
    inviteAttachment.Type = EAttachmentType::AttachmentServerInvite;
    inviteAttachment.Url = inData.ServerUUID;                        // В Url кладем СТРОГО UUID сервера
    inviteAttachment.MetaData = token;                               // В MetaData кладем токен инвайта

    SMessageData inviteMessage(dmRoomUUID, clientUUID, "");
    inviteMessage.Attachments.push_back(inviteAttachment);

    // Прокатываем сообщение через менеджер каналов, чтобы оно улетело получателю в реальном времени
    ChannelManager->newMessage(client, inviteMessage.ToJSON(), ETypeOfMessage::Message);


    response.Token = token;
    response.Response = true;
    client->sendMessage(response);
}



void ServerManager::handleAcceptInvite(const string& clientUUID, const JsonObject& data)
{
    if (clientUUID.empty()) return;

    std::lock_guard<mutex> lock(_Mutex);

    SAcceptInviteData inData(data);

    auto server = findOrAddServer(inData.ServerUUID);
    if (!server.isReadyToUse()) return;

    //TODO: доделать логику
}
*/