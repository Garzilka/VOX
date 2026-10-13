#ifndef SERVERDATA_H
#define SERVERDATA_H
#include "CoreData.h"



struct SGenerateInviteData : public SBaseMessageData
{
    QString ServerGUID;
    QString TargetUserGUID; // СКОРРЕКТИРОВАНО: ID получателя вместо ID комнаты
    QString Token;

    SGenerateInviteData(QString _ServerGUID, QString _TargetUserGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerInvite)
        : SBaseMessageData(_TypeMessage), ServerGUID(_ServerGUID), TargetUserGUID(_TargetUserGUID) {};

    SGenerateInviteData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerGUID = InJSON["ServerGUID"].toString();
        TargetUserGUID = InJSON["TargetUserGUID"].toString(); // Скорректировано
        Token = InJSON["Token"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerGUID"] = ServerGUID;
        messageObj["TargetUserGUID"] = TargetUserGUID; // Скорректировано
        messageObj["Token"] = Token;
        return messageObj;
    }
};

struct SAcceptInviteData : public SBaseMessageData
{
    QString Token;
    QString ServerGUID;
    QString ServerName;
    QString GeneralChatGUID;
    SAcceptInviteData(QString _Token, ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerAccept)
        : SBaseMessageData(_TypeMessage), Token(_Token) {};
    SAcceptInviteData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Token = InJSON["Token"].toString();
        ServerGUID = InJSON["ServerGUID"].toString();
        ServerName = InJSON["ServerName"].toString();
        GeneralChatGUID = InJSON["GeneralChatGUID"].toString();
    };
    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Token"] = Token;
        messageObj["ServerGUID"] = ServerGUID;
        messageObj["ServerName"] = ServerName;
        messageObj["GeneralChatGUID"] = GeneralChatGUID;
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

struct SServerPassport
{
    QString ServerGUID;
    QString Name;
    QString IconUrl;

    QJsonObject ToJSON() const {
        QJsonObject obj;
        obj["ServerGUID"] = ServerGUID;
        obj["Name"] = Name;
        obj["IconUrl"] = IconUrl;
        return obj;
    }

    static SServerPassport FromJSON(const QJsonObject& obj)
    {
        SServerPassport info;
        info.ServerGUID = obj["ServerGUID"].toString();
        info.Name = obj["Name"].toString();
        info.IconUrl = obj["IconUrl"].toString();
        return info;
    }
};

struct SServerPassportFor : public SBaseMessageData
{
    QString serverUUID;
    SServerPassport serverPassport;

    SServerPassportFor(ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerPassport)
        : SBaseMessageData(_TypeMessage) {};


    SServerPassportFor(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        serverUUID = InJSON["serverUUID"].toString();
        serverPassport = SServerPassport::FromJSON(InJSON["serverPassport"].toObject());
    };


    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["serverUUID"] = serverUUID;
        messageObj["serverPassport"] = serverPassport.ToJSON();

        return messageObj;
    }
};
struct SServersList : public SBaseMessageData
{
    QStringList GUIDs;

    SServersList(ETypeOfMessage _TypeMessage = ETypeOfMessage::ServerList)
        : SBaseMessageData(_TypeMessage) {};


    SServersList(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        if (InJSON.contains("GUIDs") && InJSON["GUIDs"].isArray())
        {
            QJsonArray arr = InJSON["GUIDs"].toArray();
            for (const QJsonValue& val : arr)
            {
                GUIDs.append(val.toString());
            }
        }
    };

    // Сериализация структуры в JSON
    virtual QJsonObject ToJSON() const override {
        QJsonObject messageObj = SBaseMessageData::ToJSON();

        QJsonArray guidArr;
        for (const auto& guid : GUIDs)
        {
            guidArr.append(guid);
        }
        messageObj["GUIDs"] = guidArr;

        return messageObj;
    }
};

struct SServerDetailsData : public SBaseMessageData
{
    QString ServerGUID;
    QList<QString> Channels;
    QStringList UserGUIDs;

    /*SServerDetailsData(QString _ServerGUID, ETypeOfMessage _TypeMessage = ETypeOfMessage::RequestServerDetails)
        : SBaseMessageData(_TypeMessage), ServerGUID(_ServerGUID) {};

    SServerDetailsData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ServerGUID = InJSON["ServerGUID"].toString();
        Channels.clear();
        UserGUIDs.clear();
        if (InJSON.contains("Channels") && InJSON["Channels"].isArray())
        {
            QJsonArray arr = InJSON["Channels"].toArray();
            for (const QJsonValue& val : arr)
            {
                if (val.isObject()) Channels.append(SUnifiedChatData::FromJSON(val.toObject()));
            }
        }

        if (InJSON.contains("UserGUIDs") && InJSON["UserGUIDs"].isArray())
        {
            QJsonArray arr = InJSON["UserGUIDs"].toArray();
            for (const QJsonValue& val : arr) UserGUIDs.append(val.toString());
        }
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["ServerGUID"] = ServerGUID;
        QJsonArray chanArr;
        for (const auto& chan : Channels) chanArr.append(chan.ToJSON());
        messageObj["Channels"] = chanArr;
        QJsonArray userArr;
        for (const auto& id : UserGUIDs) userArr.append(id);
        messageObj["UserGUIDs"] = userArr;
        return messageObj;
    }*/
};


#endif // SERVERDATA_H
