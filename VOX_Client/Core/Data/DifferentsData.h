#ifndef DIFFERENTSDATA_H
#define DIFFERENTSDATA_H

#include "Core/Data/CoreData.h"

struct SCyptsKeysData : public SBaseMessageData
{
    QString PublicKey;

    SCyptsKeysData(QString _PublicKey, ETypeOfMessage _TypeMessage = ETypeOfMessage::Crypto)
        : SBaseMessageData(_TypeMessage), PublicKey(_PublicKey) {};

    SCyptsKeysData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        PublicKey = InJSON["PublicKey"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["PublicKey"] = PublicKey;
        return messageObj;
    }
};

struct SAuthorizationData : public SBaseMessageData
{
    //Клиент/Сервер
    QString longToken; //LongLifeToken

    //Клиент
    QString Login;
    QString Password;
    QString Email;
    bool RememberMe = false;

    //Сервер
    QString UUID; //Ответ сервера

    SAuthorizationData(SBaseMessageData BaseData, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(BaseData), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(ETypeOfMessage _TypeMessage, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(_TypeMessage), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        longToken = InJSON["longToken"].toString();
        Login = InJSON["Login"].toString();
        Email = InJSON["Email"].toString();
        RememberMe = InJSON["RememberMe"].toBool();
        UUID = InJSON["UUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["longToken"] = longToken;
        messageObj["Login"] = Login;
        messageObj["Password"] = Password;
        messageObj["Email"] = Email;
        messageObj["RememberMe"] = RememberMe;
        return messageObj;
    }
};

struct SAppVersionData : public SBaseMessageData
{
    int ClientVersionCode = -1;
    QString Platform; // 'windows', 'linux', 'macos'

    // Поля ответа (заполняются сервером)
    int LatestVersionCode = -1;
    QString LatestVersionName;
    QString DownloadUrl;
    QString Sha256Hash;

    SAppVersionData(ETypeOfMessage _TypeMessage = static_cast<ETypeOfMessage>(24))
        : SBaseMessageData(_TypeMessage) {};

    SAppVersionData(const QJsonObject& InJSON) : SBaseMessageData(InJSON) {
        ClientVersionCode = InJSON["ClientVersionCode"].toInt();
        Platform = InJSON["Platform"].toString();
        LatestVersionCode = InJSON["LatestVersionCode"].toInt();
        LatestVersionName = InJSON["LatestVersionName"].toString();
        DownloadUrl = InJSON["DownloadUrl"].toString();
        Sha256Hash = InJSON["Sha256Hash"].toString();
    }

    virtual QJsonObject ToJSON() const override {
        QJsonObject obj = SBaseMessageData::ToJSON();
        obj["ClientVersionCode"] = ClientVersionCode;
        obj["Platform"] = Platform;
        obj["LatestVersionCode"] = LatestVersionCode;
        obj["LatestVersionName"] = LatestVersionName;
        obj["DownloadUrl"] = DownloadUrl;
        obj["Sha256Hash"] = Sha256Hash;
        return obj;
    }
};

#endif // DIFFERENTSDATA_H
