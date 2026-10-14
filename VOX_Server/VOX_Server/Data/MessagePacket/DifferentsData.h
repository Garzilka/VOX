#pragma once

#include "CoreData.h"


struct SCyptsKeysData : public SBaseMessageData
{
    string PublicKey;

    SCyptsKeysData(string _PublicKey, ETypeOfMessage _TypeMessage = ETypeOfMessage::Crypto)
        : SBaseMessageData(_TypeMessage), PublicKey(_PublicKey) {
    };

    SCyptsKeysData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        PublicKey = ToolLib::jsonGetValue<string>(InJSON, "PublicKey");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["PublicKey"] = PublicKey;
        return messageObj;
    }
};

//Временная реализация, потом нарежется на куски
struct SAuthorizationData : public SBaseMessageData
{
    //Клиент/Сервер
    string longToken; //LongLifeToken

    //Клиент
    string Login;
    string Password;
    string Email;
    bool RememberMe = false;

    //Сервер
    string UUID; //Ответ сервера
    SAuthorizationData() : SBaseMessageData(ETypeOfMessage::Login, false) {}

    SAuthorizationData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        longToken = ToolLib::jsonGetValue<string>(InJSON, "longToken");
        Login = ToolLib::jsonGetValue<string>(InJSON, "Login");
        Password = ToolLib::jsonGetValue<string>(InJSON, "Password");
        Email = ToolLib::jsonGetValue<string>(InJSON, "Email");
        RememberMe = ToolLib::jsonGetValue<bool>(InJSON, "RememberMe");
        UUID = ToolLib::jsonGetValue<string>(InJSON, "UUID");
    };

    virtual JsonObject ToJSON() const override
    {
        JsonObject messageObj = SBaseMessageData::ToJSON();

        messageObj["longToken"] = longToken;
        messageObj["Login"] = Login;
        messageObj["Password"] = Password;
        messageObj["Email"] = Email;
        messageObj["RememberMe"] = RememberMe;
        messageObj["UUID"] = UUID;
        return messageObj;
    }
};
struct SAppVersionData : public SBaseMessageData
{
    //Клиент прислал
    int ClientVersionCode = INDEX_NONE;
    string Platform{ "" };

    //Сервер ответил
    int LatestVersionCode = INDEX_NONE;
    string LatestVersionName{ "" };
    string DownloadUrl{ "" };
    string Sha256Hash{ "" };

    SAppVersionData(ETypeOfMessage _TypeMessage = ETypeOfMessage::AppVersion)
        : SBaseMessageData(_TypeMessage) {
    };

    SAppVersionData(const JsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        ClientVersionCode = ToolLib::jsonGetValue<int>(InJSON, "ClientVersionCode");
        Platform = ToolLib::jsonGetValue<string>(InJSON, "Platform");
        LatestVersionCode = ToolLib::jsonGetValue<int>(InJSON, "LatestVersionCode");
        LatestVersionName = ToolLib::jsonGetValue<string>(InJSON, "LatestVersionName");
        DownloadUrl = ToolLib::jsonGetValue<string>(InJSON, "DownloadUrl");
        Sha256Hash = ToolLib::jsonGetValue<string>(InJSON, "Sha256Hash");
    }

    virtual JsonObject ToJSON() const override
    {
        JsonObject obj = SBaseMessageData::ToJSON();
        obj["ClientVersionCode"] = ClientVersionCode;
        obj["Platform"] = Platform;
        obj["LatestVersionCode"] = LatestVersionCode;
        obj["LatestVersionName"] = LatestVersionName;
        obj["DownloadUrl"] = DownloadUrl;
        obj["Sha256Hash"] = Sha256Hash;
        return obj;
    }
};
