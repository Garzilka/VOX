/*
bool ClientManager::authViaToken(shared_ptr<Client> client, const STokenAuthData& data)
{
    if (!client) return;

    STokenAuthData tokenData(data);
    cout << "CLIENT MANAGER | Запрос авто-входа по токену для платформы:" << tokenData.DeviceInfo;

    string userUUID = "";

    if (auto Repository = _Core.getServerContext()._Repository.lock())
    {
        if (!Repository->validateRefreshToken(tokenData.RefreshToken, tokenData.DeviceInfo, userUUID)) return false;
    }

    client->setUUID(userUUID);
    AuthComplete(userUUID, client);

    SAuthResponse authResponse(userUUID);
    client->sendMessage(authResponse);
    return true;
}


void ClientManager::handleRequestOpenData(shared_ptr<Client> client, const JsonObject& data) const
{
    if (!client) return;
    SRequestOpenUsersData inData(data);
    if (inData.RequestedUUIDs.empty()) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    vector<SOpenUserData> dbUsers = Repository->getBatchUsersFromDB(inData.RequestedUUIDs);

    SRequestOpenUsersData response;
    response.Response = true;
    response.RequestedUUIDs = inData.RequestedUUIDs;

    for (auto& user : dbUsers)
    {
        if (user.PresenceStatus == EUserPresenceStatus::Offline && m_activeClients.contains(user.UUID))
        {
            user.PresenceStatus = EUserPresenceStatus::Online;
        }
        response.UsersData.push_back(user);
    }

    client->sendMessage(response);
}


void ClientManager::sendPendingRequests(shared_ptr<Client> client)
{
    if (!client) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    string myUUID = client->getUUID();
    vector<SOpenUserData> fullList = Repository->getFriendsList(myUUID);
    vector<SOpenUserData> FriendshipsList;

    for (int i = 0; i < fullList.size(); ++i)
    {
        if (fullList[i].RelationStatus == EUserRelationType::PendingSent ||
            fullList[i].RelationStatus == EUserRelationType::PendingReceived)
        {
            if (fullList[i].PresenceStatus == EUserPresenceStatus::Offline && m_activeClients.contains(fullList[i].UUID))
            {
                fullList[i].PresenceStatus = EUserPresenceStatus::Online;
            }
            FriendshipsList.push_back(fullList[i]);
        }
    }

    SFriendshipsResponse response;
    response.Response = true;
    response.Requests = FriendshipsList;

    client->sendMessage(response);
}

void ClientManager::handleFriendRequest(shared_ptr<Client> senderClient, const JsonObject& data)
{
    if (!senderClient) return;

    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    SFriendRequestData inData(data);
    auto dbResult = Repository->sendFriendRequest(senderClient->getUUID(), inData.TargetUsername);

    bool isSuccess = dbResult.first;
    string targetUUID = dbResult.second;

    SFriendRequestData responseToSender(senderClient->getUUID(), inData.TargetUsername, targetUUID);
    responseToSender.Response = isSuccess;
    senderClient->sendMessage(responseToSender);

    if (isSuccess)
    {
        sendPendingRequests(senderClient);

        if (!targetUUID.empty())
        {
            shared_ptr<Client> targetClient = findOnlineClient(targetUUID);
            if (targetClient)
            {
                SFriendRequestData notificationToTarget(senderClient->getUUID(), "");
                notificationToTarget.Response = true;
                targetClient->sendMessage(notificationToTarget);
                sendPendingRequests(targetClient);
            }
        }
    }
}

void ClientManager::handleFriendResponseProcessing(shared_ptr<Client> myClient, const JsonObject& data)
{
    if (!myClient) return;
    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    SFriendResponseData inData = SFriendResponseData(data);
    string myUUID = myClient->getUUID();
    string targetUUID = inData.TargetUUID;

    bool isSuccess = Repository->handleFriendResponse(myUUID, targetUUID, inData.Action);

    SFriendResponseData responseToMe(targetUUID, inData.Action);
    responseToMe.Response = isSuccess;
    myClient->sendMessage(responseToMe);

    if (isSuccess)
    {
        handleGetFriendsList(myClient);
        sendPendingRequests(myClient);

        shared_ptr<Client> targetClient = findOnlineClient(targetUUID);
        if (targetClient)
        {
            SFriendResponseData notificationToTarget(myUUID, inData.Action);
            notificationToTarget.Response = true;
            targetClient->sendMessage(notificationToTarget);
            handleGetFriendsList(targetClient);
            sendPendingRequests(targetClient);
        }
    }
}

void ClientManager::handleGetFriendsList(shared_ptr<Client> client)
{
    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    string myUUID = client->getUUID();
    vector<SOpenUserData> fullList = Repository->getFriendsList(myUUID);
    vector<SOpenUserData> acceptedFriendsOnly;

    for (int i = 0; i < fullList.size(); ++i)
    {
        if (fullList[i].RelationStatus == 2)
        {
            if (fullList[i].PresenceStatus == EUserPresenceStatus::Offline && m_activeClients.contains(fullList[i].UUID))
            {
                fullList[i].PresenceStatus = EUserPresenceStatus::Online;
            }
            acceptedFriendsOnly.push_back(fullList[i]);
        }
    }

    SFriendsListData response;
    response.Response = true;
    response.Friends = acceptedFriendsOnly; // Отправляем клиенту ТОЛЬКО реальных друзей

    client->sendMessage(response);
}



void ClientManager::handleSearchUsers(shared_ptr<Client> client, const JsonObject& data)
{
    auto Repository = _Core.getServerContext()._Repository.lock();
    if (!Repository) return;

    SSearchUsersData inData(data);
    string myUUID = client->getUUID();

    vector<SOpenUserData> searchResults = Repository->searchUsersByUsername(myUUID, toLower(inData.SearchQuery));

    SSearchUsersData response(inData.SearchQuery);
    response.Response = true;
    response.Results = searchResults;

    client->sendMessage(response);
}*/

