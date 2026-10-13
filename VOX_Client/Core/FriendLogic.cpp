#include "connection.h"
#include "Core/Data/DataIncluds.h"
#include "Core/UIManager/uimanager.h"


void QConnection::requestSearchUser(const QString &UserName)
{
    if(!DebugMode)
    {
        SSearchUsersData sendData(UserName);
        sendMessage(sendData);
    }
}



void QConnection::ResponseSearchUsers(QJsonObject &JSONObject)
{
    /*SSearchUsersData searchData(JSONObject);
    QVariantList formattedResults;

    for (const SOpenUserData& user : searchData.Results)
    {
        QVariantMap userMap;
        userMap["guid"] = user.GUID;
        userMap["username"] = user.Username;
        userMap["displayName"] = user.DisplayName;
        userMap["avatarUrl"] = user.AvatarUrl;
        userMap["presenceStatus"] = user.PresenceStatus; // Числовой статус

        formattedResults.append(userMap);
    }
    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr) friendMgr->setSearchResults(formattedResults);*/
}

void QConnection::ResponseFriendsList(QJsonObject &JSONObject)
{
    /*SFriendsListData friendsData(JSONObject);
    QVariantList formattedFriends;

    for (const SOpenUserData& friendInfo : friendsData.Friends)
    {
        if (friendInfo.RelationStatus != 2)
        {
            continue;
        }

        QVariantMap friendMap;
        friendMap["guid"] = friendInfo.GUID;
        friendMap["username"] = friendInfo.Username;
        friendMap["displayName"] = friendInfo.DisplayName;
        friendMap["avatarUrl"] = friendInfo.AvatarUrl;
        friendMap["presenceStatus"] = friendInfo.PresenceStatus; // 0-4
        friendMap["relationStatus"] = friendInfo.RelationStatus;

        formattedFriends.append(friendMap);
    }

    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr) friendMgr->setFriendsList(formattedFriends);*/
}

void QConnection::ResponseFriendships(QJsonObject &JSONObject)
{
    /*SFriendshipsResponse pendingData(JSONObject);
    QVariantList formattedRequests;

    for (const SOpenUserData& req : pendingData.Requests)
    {
        QVariantMap reqMap;
        reqMap["guid"] = req.GUID;
        reqMap["username"] = req.Username;
        reqMap["displayName"] = req.DisplayName;
        reqMap["avatarUrl"] = req.AvatarUrl;
        reqMap["presenceStatus"] = req.PresenceStatus;
        reqMap["relationStatus"] = req.RelationStatus;

        formattedRequests.append(reqMap);
    }
    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr) friendMgr->setPendingRequests(formattedRequests);*/
}

void QConnection::requestFriend(const QString &GUID, const QString &UserName)
{
    if(!DebugMode) {
        SFriendRequestData sendData(UUID, UserName, GUID);
        sendMessage(sendData);
    }
}

void QConnection::requestFriendAction(const QString &targetGUID, int action)
{
    if(!DebugMode)
    {
        SFriendResponseData sendData(targetGUID, action);
        sendMessage(sendData);
    }
}
