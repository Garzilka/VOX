#pragma once
#include "Data/MessagesIncluds.h"


class IMakeServerService {
public:
    virtual void makeServer(const string userUUID, const JsonObject& obj) = 0;
};


class IDBServerService {
public:
    virtual void requestMakeServerInDB(const string userUUID, const JsonObject& obj) = 0;
    virtual void requestPassportInDB(const string& serverUUID) = 0;
    virtual void requestDetailsInDB(const string& serverUUID) = 0;
};

class ICacheServerService {
public:
    virtual SGettedServerState findOrAddServer(const string& userUUID) = 0;
    virtual void setServerToReady(const string& serverName, const string& beforeUUID) = 0;
};

class IPassportService {
public:
    virtual bool setServerPassport(SServerPassport) = 0;
    virtual bool getServerPassport(SServerPassport&, const string&) = 0;
};

class IDetailsService {
public:
    virtual bool setServerDetails(const SServerDetailsData&) = 0;
    virtual bool getServerDetails(SServerDetailsData&, const string&) = 0;
};