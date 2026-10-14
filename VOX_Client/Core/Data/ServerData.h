#ifndef SERVERDATA_H
#define SERVERDATA_H
#include "CoreData.h"




struct SServerPassport : public SBaseMessageData
{
    QString ServerGUID;
    QString Name;
    QString IconUrl;

    SServerPassport(QString _ServerGUID) : ServerGUID(_ServerGUID), SBaseMessageData(ETypeOfMessage::ServerPassport) {};

    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["ServerGUID"] = ServerGUID;
        obj["Name"] = Name;
        obj["IconUrl"] = IconUrl;
        return obj;
    }

    static SServerPassport FromJSON(const QJsonObject& obj)
    {
        SServerPassport info("");
        info.ServerGUID = obj["ServerGUID"].toString();
        info.Name = obj["Name"].toString();
        info.IconUrl = obj["IconUrl"].toString();
        return info;
    }
};

struct SServerRoleData
{
    QString RoleUUID;
    QString Name;
    QString Color;
    qint64 Permissions = 0;
    qint16 Position = 0;

    QJsonObject toJSON() const {
        QJsonObject obj;
        obj["RoleUUID"] = RoleUUID;
        obj["Name"] = Name;
        obj["Color"] = Color;
        obj["Permissions"] = Permissions;
        obj["Position"] = Position;
        return obj;
    }

    static SServerRoleData fromJSON(const QJsonObject& json) {
        SServerRoleData data;
        data.RoleUUID = json["RoleUUID"].toString();
        data.Name = json["Name"].toString();
        data.Color = json["Color"].toString();
        // В Qt json числа возвращаются как double, поэтому приводим типы:
        data.Permissions = static_cast<qint64>(json["Permissions"].toDouble());
        data.Position = static_cast<qint16>(json["Position"].toInt());
        return data;
    }
};

struct SServerMemberShortData
{
    QString UserUUID;
    QString RoleUUID;

    QJsonObject toJSON() const {
        QJsonObject obj;
        obj["UserUUID"] = UserUUID;
        obj["RoleUUID"] = RoleUUID;
        return obj;
    }

    static SServerMemberShortData fromJSON(const QJsonObject& json) {
        SServerMemberShortData data;
        data.UserUUID = json["UserUUID"].toString();
        data.RoleUUID = json["RoleUUID"].toString();
        return data;
    }
};

struct SServerDetailsData : public SBaseMessageData
{
    QString ServerUUID{""};
    QList<QString> Channels{};
    QList<SServerRoleData> RoleData{};
    QList<SServerMemberShortData> MembersData{};

    SServerDetailsData(ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerDetails)
        : SBaseMessageData(_TypeMessage) {}

    SServerDetailsData(QString _ServerUUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerDetails)
        : SBaseMessageData(_TypeMessage), ServerUUID(_ServerUUID) {}

    SServerDetailsData(const QJsonObject& inJSON) : SBaseMessageData(inJSON)
    {
        ServerUUID = inJSON["ServerUUID"].toString();

        Channels.clear();
        RoleData.clear();
        MembersData.clear();

        if (inJSON.contains("Channels") && inJSON["Channels"].isArray())
        {
            QJsonArray arr = inJSON["Channels"].toArray();
            Channels.reserve(arr.size());
            for (const QJsonValue& item : arr)
            {
                Channels.append(item.toString());
            }
        }

        if (inJSON.contains("RoleData") && inJSON["RoleData"].isArray())
        {
            QJsonArray arr = inJSON["RoleData"].toArray();
            RoleData.reserve(arr.size());
            for (const QJsonValue& item : arr)
            {
                if (item.isObject()) {
                    RoleData.append(SServerRoleData::fromJSON(item.toObject()));
                }
            }
        }

        if (inJSON.contains("MembersData") && inJSON["MembersData"].isArray())
        {
            QJsonArray arr = inJSON["MembersData"].toArray();
            MembersData.reserve(arr.size());
            for (const QJsonValue& item : arr)
            {
                if (item.isObject()) {
                    MembersData.append(SServerMemberShortData::fromJSON(item.toObject()));
                }
            }
        }
    }

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerUUID"] = ServerUUID;

        QJsonArray channelsArr;
        for (const QString& channel : Channels) {
            channelsArr.append(channel);
        }
        messageObj["Channels"] = channelsArr;

        QJsonArray roleDataArr;
        for (const SServerRoleData& role : RoleData) {
            roleDataArr.append(role.toJSON());
        }
        messageObj["RoleData"] = roleDataArr;

        QJsonArray membersArr;
        for (const SServerMemberShortData& member : MembersData) {
            membersArr.append(member.toJSON());
        }
        messageObj["MembersData"] = membersArr;

        return messageObj;
    }
};


struct SGenerateInviteData : public SBaseMessageData
{
    QString ServerGUID;
    QString TargetUserGUID;
    QString Token;

    SGenerateInviteData(QString _ServerGUID, QString _TargetUserGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerInvite)
        : SBaseMessageData(_TypeMessage), ServerGUID(_ServerGUID), TargetUserGUID(_TargetUserGUID) {};

    SGenerateInviteData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerGUID = InJSON["ServerGUID"].toString();
        TargetUserGUID = InJSON["TargetUserGUID"].toString();
        Token = InJSON["Token"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerGUID"] = ServerGUID;
        messageObj["TargetUserGUID"] = TargetUserGUID;
        messageObj["Token"] = Token;
        return messageObj;
    }
};

struct SAcceptInviteData : public SBaseMessageData
{
    QString Token;
    QString ServerGUID;

    SAcceptInviteData(QString _Token, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerAccept)
        : SBaseMessageData(_TypeMessage), Token(_Token) {};
    SAcceptInviteData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Token = InJSON["Token"].toString();
        ServerGUID = InJSON["ServerGUID"].toString();
    };
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Token"] = Token;
        messageObj["ServerGUID"] = ServerGUID;
        return messageObj;
    }
};

struct SCreateServerData : public SBaseMessageData
{
    QString ServerName;
    QString OwnerGUID;
    QString ServerGUID;
    SCreateServerData(QString _ServerName, QString _OwnerGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerCreate)
        : SBaseMessageData(_TypeMessage), ServerName(_ServerName), OwnerGUID(_OwnerGUID) {};

    SCreateServerData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerName = InJSON["ServerName"].toString();
        OwnerGUID = InJSON["OwnerGUID"].toString();
        ServerGUID = InJSON["ServerGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerName"] = ServerName;
        messageObj["OwnerGUID"] = OwnerGUID;
        messageObj["ServerGUID"] = ServerGUID;
        return messageObj;
    }
};

#endif // SERVERDATA_H
