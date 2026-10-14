#ifndef USERDATA_H
#define USERDATA_H

#include "Core/Data/CoreData.h"

struct SSearchUsersData : public SBaseMessageData
{
    QString SearchQuery;
    QList<QString> Results;
    SSearchUsersData(QString _SearchQuery, ETypeOfMessage _TypeMessage = ETypeOfMessage::SearchUsers)
        : SBaseMessageData(_TypeMessage), SearchQuery(_SearchQuery) {};

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SearchQuery"] = SearchQuery;
        QJsonArray arr;
        for(const auto& user : Results)
        {
            arr.append(user);
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
                Results.append(value.toString());
            }
        }
    };
};


struct SUserPassport : public SBaseMessageData
{
    QString UUID;
    QString DisplayName;
    QString AvatarUrl;
    int PresenceStatus = EUserPresenceStatus::Offline;
    QList<QString> joinedServersUUID;

    SUserPassport() : SBaseMessageData(ETypeOfMessage::UserPassport) {};

    SUserPassport(const QJsonObject& obj) : SBaseMessageData(obj)
    {
        UUID = obj["UUID"].toString();
        DisplayName = obj["DisplayName"].toString();
        AvatarUrl = obj["AvatarUrl"].toString();
        PresenceStatus = obj["PresenceStatus"].toInt();

        if (obj.contains("joinedServersUUID") && obj["joinedServersUUID"].isArray())
        {
            QJsonArray arr = obj["joinedServersUUID"].toArray();
            for (const QJsonValue& value : arr)
            {
                joinedServersUUID.append(value.toString());
            }
        }
    }

    QJsonObject ToJSON() const
    {
        QJsonObject obj = SBaseMessageData::ToJSON();
        obj["UUID"] = UUID;
        obj["DisplayName"] = DisplayName;
        obj["AvatarUrl"] = AvatarUrl;
        obj["PresenceStatus"] = PresenceStatus;


        QJsonArray arr;
        for(const auto& user : joinedServersUUID)
        {
            arr.append(user);
        }
        obj["joinedServersUUID"] = arr;
        return obj;
    }
};

struct SUserDetails
{
    QString aboutMe;
    QString bannerUrl;
    QString avatarFrameUrl;
};

struct SPeerCustomSettings
{
    QString note;
    int voiceVolume = 100;
    bool isMuted = false;

    SPeerCustomSettings(const QJsonObject& InJSON)
    {
        note = InJSON["note"].toString();
        voiceVolume = InJSON["voiceVolume"].toInt();
        isMuted = InJSON["isMuted"].toInt();
    }

    QJsonObject ToJSON() const
    {
        QJsonObject messageObj;
        messageObj["note"] = note;
        messageObj["voiceVolume"] = voiceVolume;
        messageObj["isMuted"] = isMuted;

        return messageObj;
    }

};

struct SUserRelations : public SBaseMessageData
{
    QString userOwnerUUID;
    QSet<QString> IncRequestFriend;
    QSet<QString> OutcRequestFrined;
    QSet<QString> friendsUUID;
    QSet<QString> bannedUUID;
    QHash<QString, SPeerCustomSettings> peerSettings;

    SUserRelations(ETypeOfMessage _TypeMessage = ETypeOfMessage::Relations)
        : SBaseMessageData(_TypeMessage) {
        };

    SUserRelations(const QJsonObject& InJSON, ETypeOfMessage _TypeMessage = ETypeOfMessage::Relations)
        : SBaseMessageData(_TypeMessage)
    {
        userOwnerUUID = InJSON["userOwnerUUID"].toString();

        if (InJSON.contains("IncRequestFriend") && InJSON["IncRequestFriend"].isArray())
        {
            QJsonArray arr = InJSON["IncRequestFriend"].toArray();
            for (const QJsonValue& value : arr)
            {
                IncRequestFriend.insert(value.toString());
            }
        }

        if (InJSON.contains("OutcRequestFrined") && InJSON["OutcRequestFrined"].isArray())
        {
            QJsonArray arr = InJSON["OutcRequestFrined"].toArray();
            for (const QJsonValue& value : arr)
            {
                OutcRequestFrined.insert(value.toString());
            }
        }

        if (InJSON.contains("friendsUUID") && InJSON["friendsUUID"].isArray())
        {
            QJsonArray arr = InJSON["friendsUUID"].toArray();
            for (const QJsonValue& value : arr)
            {
                friendsUUID.insert(value.toString());
            }
        }

        if (InJSON.contains("bannedUUID") && InJSON["bannedUUID"].isArray())
        {
            QJsonArray arr = InJSON["bannedUUID"].toArray();
            for (const QJsonValue& value : arr)
            {
                bannedUUID.insert(value.toString());
            }
        }
    }

    QJsonObject ToJSON() const
    {
        QJsonObject messageObj;

        messageObj["userOwnerUUID"] = userOwnerUUID;

        QJsonArray arr;
        for(const auto& user : IncRequestFriend)
        {
            arr.append(user);
        }
        messageObj["IncRequestFriend"] = arr;

        QJsonArray arr1;
        for(const auto& user : OutcRequestFrined)
        {
            arr1.append(user);
        }
        messageObj["OutcRequestFrined"] = arr1;

        QJsonArray arr2;
        for(const auto& user : friendsUUID)
        {
            arr.append(user);
        }
        messageObj["friendsUUID"] = arr2;
        QJsonArray arr3;
        for(const auto& user : bannedUUID)
        {
            arr.append(user);
        }
        messageObj["bannedUUID"] = arr3;

        return messageObj;
    }

};
#endif // USERDATA_H
