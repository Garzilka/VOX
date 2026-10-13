#pragma once
#include "Rules/CheckDBrule.h"
#include "Data/BDTaskTypes.h"

class Core;

class Repository
{
private:
    shared_ptr<pqxx::connection> _SQL_Connection;
    CheckDBrule* CurrentRule = nullptr;

    // Внутренний метод для регистрации подготовленных запросов в PostgreSQL
    void PrepareQueries();

public:
    Repository() {};
    virtual ~Repository() = default;

    Repository& operator=(const Repository&) = delete;
    Repository(const Repository&) = delete;

    bool Run();

    bool loadAppVersionData(SAppVersionData& platform);

    // ==========================================
    // Работа с авторизацией
    // ==========================================
    string RegisterNewAccount(const SAuthorizationData& Data);
    string CheckAuth(const SAuthorizationData& Data);
    bool HasAccount(const string& _Username);

    // ==========================================
    // Работа с голосовыми каналами
    // ==========================================
    bool updateVoiceStateInDB(const string& userUUID, const string& serverUUID, const string& channelUUID);
    bool removeVoiceStateFromDB(const string& userUUID);
    unordered_map<string, vector<string>> getServerVoiceStates(const string& serverUUID);

    // ==========================================
    // Работа с блоком друзей
    // ==========================================
    vector<SUserPassport> getFriendsList(const string& myUUID);
    vector<SUserPassport> searchUsersByUsername(const string& myUUID, const string& searchQuery);
    vector<SUserPassport> getBatchUsersFromDB(const vector<string>& UUIDs);
    pair<bool, string> sendFriendRequest(const string& senderUUID, const string& targetUsername);
    bool handleFriendResponse(const string& myUUID, const string& targetUUID, int action);

    // ==========================================
    // Работа с каналами
    // ==========================================
    string findPrivateChannel(const string& senderUUID, const string& targetUUID);
    bool createPrivateChannel(const string& roomUUID, const string& senderUUID, const string& targetUUID);
    string getOrCreatePrivateChannel(const string& senderUUID, const string& targetUUID);
    vector<SChannelPassport> getUserDirectChats(const string& userUUID);
    bool createServerChannelInDB(const string& channelUUID, const string& serverUUID, const string& name, int chatType);

    // ==========================================
    // Система сообщений
    // ==========================================
    bool saveMessage(const SMessageData& msg);
    vector<SMessageData> loadHistoryFromDB(const string& roomUUID, int limit, int offset);
    bool isChannelExists(const string& roomUUID);

    // ==========================================
    // Работа с клиентскими сессиями
    // ==========================================
    bool generateAndSaveSession(const string& userUUID, const string& deviceInfo, string& resultUUID);
    bool validateRefreshToken(const string& token, const string& platform, string& resultUUID);
    void deleteSessionByToken(const string& token);

    // ==========================================
    // Работа с серверами
    // ==========================================
    bool createServerInDB(const SMakeServerTask& result_MakeTask, SServerPassport& Result);
    bool createInviteInDB(const string& token, const string& serverUUID, const string& inviterUUID);
    vector<string> getUserServersUUIDs(const string& userUUID);
    bool acceptInviteInDB(const string& token, const string& userUUID);

    bool loadServerPassport(SServerPassport& _ServerPasport);
    bool loadServerChannelsUUIDs(const string& serverUUID, vector<string>& _Result);
    bool loadServerRoles(const string& serverUUID, vector<SServerRoleData>& _Result);
    bool loadServerMembersWithRoles(const string& serverUUID, vector<SServerMemberShortData>& _Result);


};
