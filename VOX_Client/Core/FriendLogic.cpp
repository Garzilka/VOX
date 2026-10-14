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
    SSearchUsersData searchData(JSONObject);
    QVariantList formattedResults;

    for (const QString& userUUID : searchData.Results)
    {
        formattedResults.append(userUUID);
    }
    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr) friendMgr->setSearchResults(formattedResults);
}

void QConnection::ResponseRelations(QJsonObject &JSONObject)
{
    qInfo() << "ResponseRelations";
    SUserRelations friendsData(JSONObject);

    QVariantList formattedFriends;
    for (const QString& friendUUID : friendsData.friendsUUID)
    {
        formattedFriends.append(friendUUID);
    }

    QVariantList formattedPending;
    for (const QString& incUUID : friendsData.IncRequestFriend)
    {
        QVariantMap requestMap;
        requestMap["guid"] = incUUID;
        requestMap["direction"] = "incoming";
        formattedPending.append(requestMap);
    }

    QJsonArray outcArr = JSONObject["OutcRequestFrined"].toArray();
    for (const QJsonValue& value : outcArr)
    {
        QVariantMap requestMap;
        requestMap["guid"] = value.toString();
        requestMap["direction"] = "outgoing";
        formattedPending.append(requestMap);
    }

    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr)
    {
        friendMgr->setFriendsList(formattedFriends);
        friendMgr->setPendingRequests(formattedPending);
    }

    QStringList allLazyUUIDs;
    allLazyUUIDs.reserve(friendsData.friendsUUID.size() + friendsData.IncRequestFriend.size() + outcArr.size());

    for(const auto& id : friendsData.friendsUUID) allLazyUUIDs.append(id);
    for(const auto& id : friendsData.IncRequestFriend) allLazyUUIDs.append(id);
    for(const auto& val : outcArr) allLazyUUIDs.append(val.toString());

    if(!allLazyUUIDs.isEmpty())
    {
        requestUserPassport(allLazyUUIDs);
    }
}

void QConnection::requestFriend(const QString &GUID, const QString &UserName)
{
    if(!DebugMode)
    {
        SFriendRequest sendData(UUID, UserName, GUID);
        sendMessage(sendData);
    }
}

void QConnection::requestFriendAction(const QString &targetGUID, int action)
{
    /*if(!DebugMode)
    {
        SFriendResponseData sendData(targetGUID, action);
        sendMessage(sendData);
    }*/
}
