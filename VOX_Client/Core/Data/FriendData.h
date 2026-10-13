#ifndef FRIENDDATA_H
#define FRIENDDATA_H
#include "CoreData.h"

struct SRequestUserPassport : public SBaseMessageData
{
    QString userUUID;
    SUserPassport userPassport;

    SRequestUserPassport(ETypeOfMessage _TypeMessage = ETypeOfMessage::UserPassport)
        : SBaseMessageData(_TypeMessage) {};

    SRequestUserPassport(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        userUUID = InJSON["userUUID"].toString();
        userPassport = SUserPassport::FromJSON(InJSON["userPassport"].toObject());
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["userUUID"] = userUUID;
        messageObj["RequestedGUIDs"] = userPassport.ToJSON();
        return messageObj;
    }
};

struct SSearchUsersData : public SBaseMessageData
{
    QString SearchQuery;
    QList<SUserPassport> Results;
    SSearchUsersData(QString _SearchQuery, ETypeOfMessage _TypeMessage = ETypeOfMessage::SearchUsers)
        : SBaseMessageData(_TypeMessage), SearchQuery(_SearchQuery) {};
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SearchQuery"] = SearchQuery;
        QJsonArray arr;
        for(const auto& user : Results) {
            arr.append(user.ToJSON());
        }
        messageObj["Results"] = arr;
        return messageObj;
    }
    SSearchUsersData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SearchQuery = InJSON["SearchQuery"].toString();
        Results.clear();
        if (InJSON.contains("Results") && InJSON["Results"].isArray())
        {
            QJsonArray arr = InJSON["Results"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    Results.append(SUserPassport::FromJSON(value.toObject()));
                }
            }
        }
    };
};


struct SFriendshipsResponse : public SBaseMessageData
{
    QList<SUserPassport> Requests;
    SFriendshipsResponse(ETypeOfMessage _TypeMessage = ETypeOfMessage::Friendships)
        : SBaseMessageData(_TypeMessage) {};
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        QJsonArray arr;
        for(const auto& user : Requests)
        {
            arr.append(user.ToJSON());
        }
        messageObj["Requests"] = arr;
        return messageObj;
    }
    SFriendshipsResponse(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Requests.clear();
        if (InJSON.contains("Requests") && InJSON["Requests"].isArray())
        {
            QJsonArray arr = InJSON["Requests"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    Requests.append(SUserPassport::FromJSON(value.toObject()));
                }
            }
        }
    };
};

struct SFriendRequestData : public SBaseMessageData
{
    QString SenderGUID;
    QString TargetUsername;
    QString TargetGUID;

    SFriendRequestData(QString _SenderGUID, QString _TargetUsername, QString _TargetGUID = "", ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendRequest)
        : SBaseMessageData(_TypeMessage), SenderGUID(_SenderGUID), TargetUsername(_TargetUsername), TargetGUID(_TargetGUID) {};

    SFriendRequestData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SenderGUID = InJSON["SenderGUID"].toString();
        TargetUsername = InJSON["TargetUsername"].toString();
        TargetGUID = InJSON["TargetGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SenderGUID"] = SenderGUID;
        messageObj["TargetUsername"] = TargetUsername;
        messageObj["TargetGUID"] = TargetGUID;
        return messageObj;
    }
};

struct SFriendResponseData : public SBaseMessageData
{
    QString TargetGUID;
    bool Action = false;        // 0 - Принять, 1 - Отклонить/Удалить

    SFriendResponseData(QString _TargetGUID, int _Action, ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendAccept)
        : SBaseMessageData(_TypeMessage), TargetGUID(_TargetGUID), Action(_Action) {};

    SFriendResponseData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        TargetGUID = InJSON["TargetGUID"].toString();
        Action = InJSON["Action"].toBool();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["TargetGUID"] = TargetGUID;
        messageObj["Action"] = Action;
        return messageObj;
    }
};

struct SFriendsListData : public SBaseMessageData
{
    QList<SUserPassport> Friends;

    SFriendsListData(ETypeOfMessage _TypeMessage = ETypeOfMessage::Friends)
        : SBaseMessageData(_TypeMessage) {};

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        QJsonArray arr;
        for(const auto& user : Friends) {
            arr.append(user.ToJSON());
        }
        messageObj["Friends"] = arr;
        return messageObj;
    }

    SFriendsListData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Friends.clear();
        if (InJSON.contains("Friends") && InJSON["Friends"].isArray())
        {
            QJsonArray arr = InJSON["Friends"].toArray();
            for (const QJsonValue& value : arr)
            {

                if (value.isObject())
                {
                    Friends.append(SUserPassport::FromJSON(value.toObject()));
                }
            }
        }
    };
};

#endif // FRIENDDATA_H
