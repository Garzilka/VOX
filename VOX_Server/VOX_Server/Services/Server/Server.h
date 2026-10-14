#pragma once
#include "Data/MessagesIncluds.h"

enum class EServerState
{
    NOT_STARTED,
    LOADING,
    READY,
    FAILED
};

struct SGettedServerState
{
    EServerState _State;
    shared_ptr<class Server> _MakedServer;

    SGettedServerState(EServerState result_State) : _State(result_State) {};

    bool isReadyToUse() { return (_MakedServer && _State == EServerState::READY); }
};

class Server 
{

private:
    SServerPassport _ServerPassport;
    SServerDetailsData _ServerDetailsData;
    unordered_set<string> _PassportPendingClients;
    unordered_set<string> _DetailsPendingClients;

    mutable mutex _ServerMutex;

    bool _isValid = true;           
    bool _isDetailsLoaded = false;

    bool LoadDetails();
public:
    explicit Server() = default;
    bool getServerPassport(SServerPassport& newPassport, const string& clientUUID);
    bool getServerDetails(SServerDetailsData& newServerDetailsData, const string& clientUUID);

    bool setServerPassport(SServerPassport newServerPasport, vector<string>& result_Pendings);
    bool setServerDetails(const SServerDetailsData& newServerDetailsData, vector<string>& result_Pendings);
    void addPendingClients(const string& clientUUID);

    bool addChannel(const string& _Channel_UUID);
    void addMember(const string& _Client_UUID);
    void removeMember(const string& _Client_UUID);

    bool isValid() const { return _isValid; }
    bool isDetailsLoaded() const { return _isDetailsLoaded; }
    bool isDetailsNeedLoaded() const;
    vector<string> getMembers();
    void fillPendingList(vector<string>& result, unordered_set<string>& source);
};
