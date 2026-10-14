#include "Repository.h"

#include "Data/Common.h"
#include "Data/MessagesIncluds.h"
#include "Libs/Crypto/CryptoLib.h"

#include "Rules/CheckUsersRule.h"
#include "Rules/CheckServersRule.h"
#include "Rules/CheckSessionsRule.h"
#include "Rules/CheckChannelsRule.h"
#include "Rules/CheckServerRolesRule.h"
#include "Rules/CheckServerInvitesRule.h"
#include "Rules/CheckChannelMembersRule.h"
#include "Rules/CheckMessagesRule.h"
#include "Rules/CheckFriendsRule.h"
#include "Rules/CheckAppVersionsRule.h"
#include "Rules/CheckVoiceStatesRule.h"
#include "Rules/CheckServerMembersRule.h"


// =========================================================================
// Вспомогательные функции на чистом C++ для замены Qt-утилит
// =========================================================================

static string toLowerTrimmed(string str) {
    // Trim в начале
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    // Trim в конце
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), str.end());
    // В нижний регистр
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    return str;
}

// Простейшая генерация UUID v4 без сторонних библиотек
static string generateUUID() 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4"; // v4 uuid
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}


static string getDateTimeOffsetInDays(int days) 
{
    auto now = std::chrono::system_clock::now();
    auto future = now + std::chrono::hours(24 * days);
    std::time_t future_time = std::chrono::system_clock::to_time_t(future);

    std::tm time_info;
    if (gmtime_s(&time_info, &future_time) != 0)
    {
        spdlog::error("[Repository] Не удалось сконвертировать время.");
        return "";
    }

    std::stringstream ss;
    ss << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

static bool isExpired(const std::string& timestampStr)
{
    std::stringstream ss(timestampStr);
    std::chrono::sys_seconds tp;
    std::chrono::from_stream(ss, "%Y-%m-%d %H:%M:%S", tp);
    if (ss.fail()) return true;

    return std::chrono::system_clock::now() > tp;
}

// =========================================================================
// Инициализация подготовленных запросов
// =========================================================================
void Repository::PrepareQueries()
{
    if (!_SQL_Connection) return;

    _SQL_Connection->prepare("get_latest_version",
        "SELECT version_code, version_name, download_url, sha256_hash FROM app_versions WHERE platform = $1 ORDER BY version_code DESC LIMIT 1;");

    _SQL_Connection->prepare("check_auth", "SELECT id, password_hash, salt FROM users WHERE username = $1;");

    _SQL_Connection->prepare("insert_user",
        "INSERT INTO users (username, display_name, email, password_hash, salt, status) VALUES ($1, $2, $3, $4, $5, 0) RETURNING id;");

    _SQL_Connection->prepare("has_account", "SELECT EXISTS(SELECT 1 FROM users WHERE username = $1);");

    _SQL_Connection->prepare("search_users",
        "SELECT id, username, display_name, avatar_url, status FROM users WHERE username ILIKE '%' || $1 || '%' AND id <> $2 LIMIT 20;");

    _SQL_Connection->prepare("find_user_by_name", "SELECT id FROM users WHERE username = $1;");

    _SQL_Connection->prepare("check_friendship_exists",
        "SELECT status FROM friends WHERE (user_id = $1 AND friend_id = $2) OR (user_id = $2 AND friend_id = $1);");

    _SQL_Connection->prepare("insert_friend_request", "INSERT INTO friends (user_id, friend_id, status) VALUES ($1, $2, 'pending');");

    _SQL_Connection->prepare("accept_friend", "UPDATE friends SET status = 'accepted' WHERE user_id = $1 AND friend_id = $2 AND status = 'pending';");

    _SQL_Connection->prepare("delete_friend", "DELETE FROM friends WHERE (user_id = $1 AND friend_id = $2) OR (user_id = $2 AND friend_id = $1);");

    _SQL_Connection->prepare("find_private_channel",
        "SELECT cm.channel_id FROM channel_members cm JOIN channels c ON cm.channel_id = c.id WHERE c.type = 0 AND (cm.user_id = $1 OR cm.user_id = $2) GROUP BY cm.channel_id HAVING count(cm.user_id) = 2;");

    _SQL_Connection->prepare("insert_channel", "INSERT INTO channels (id, type) VALUES ($1, 0);");

    _SQL_Connection->prepare("insert_channel_member", "INSERT INTO channel_members (channel_id, user_id) VALUES ($1, $2);");

    _SQL_Connection->prepare("get_user_direct_chats",
        "SELECT c.id, c.type, c.name FROM channel_members cm JOIN channels c ON cm.channel_id = c.id WHERE cm.user_id = $1 AND c.type IN (0, 1);");

    _SQL_Connection->prepare("get_channel_participants", "SELECT user_id FROM channel_members WHERE channel_id = $1;");

    _SQL_Connection->prepare("get_last_channel_message", "SELECT message_text FROM messages WHERE channel_id = $1 ORDER BY created_at DESC LIMIT 1;");

    _SQL_Connection->prepare("insert_server_channel", "INSERT INTO channels (id, type, server_id, name) VALUES ($1, $2, $3, $4);");

    _SQL_Connection->prepare("insert_message", "INSERT INTO messages (channel_id, sender_id, message_text) VALUES ($1, $2, $3) RETURNING id;");

    _SQL_Connection->prepare("insert_attachment", "INSERT INTO message_attachments (message_id, attachment_type, url, meta_data) VALUES ($1, $2, $3, $4);");

    _SQL_Connection->prepare("load_history", "SELECT id, sender_id, message_text FROM messages WHERE channel_id = $1 ORDER BY created_at DESC LIMIT $2 OFFSET $3;");

    _SQL_Connection->prepare("is_channel_exists", "SELECT 1 FROM channels WHERE id = $1 LIMIT 1;");

    _SQL_Connection->prepare("insert_session", "INSERT INTO user_sessions (user_id, refresh_token, device_info, expires_at) VALUES ($1, $2, $3, $4);");

    _SQL_Connection->prepare("validate_session", "SELECT user_id, expires_at FROM user_sessions WHERE refresh_token = $1;");

    _SQL_Connection->prepare("update_session_platform", "UPDATE user_sessions SET device_info = $1 WHERE refresh_token = $2;");

    _SQL_Connection->prepare("delete_session", "DELETE FROM user_sessions WHERE refresh_token = $1;");

    _SQL_Connection->prepare(
        "insert_server",
        "INSERT INTO servers (name, owner_id) VALUES ($1, $2) RETURNING id, name, COALESCE(icon_url, '');"
    );

    _SQL_Connection->prepare("insert_server_member", "INSERT INTO server_members (server_id, user_id) VALUES ($1, $2);");

    _SQL_Connection->prepare("insert_server_channel_full", "INSERT INTO channels (id, type, server_id, name) VALUES ($1, $2, $3, $4);");

    _SQL_Connection->prepare("get_server_members_UUIDs", "SELECT user_id FROM server_members WHERE server_id = $1;");

    _SQL_Connection->prepare("insert_invite", "INSERT INTO server_invites (token, server_id, inviter_id) VALUES ($1, $2, $3);");

    _SQL_Connection->prepare("upsert_voice_state", R"(
        INSERT INTO voice_states (user_id, server_id, channel_id) VALUES ($1, $2, $3)
        ON CONFLICT (user_id) DO UPDATE SET server_id = $2, channel_id = $3, joined_at = CURRENT_TIMESTAMP;
    )");

    _SQL_Connection->prepare("delete_voice_state", "DELETE FROM voice_states WHERE user_id = $1;");

    _SQL_Connection->prepare("get_server_voice_states", "SELECT channel_id, user_id FROM voice_states WHERE server_id = $1;");

    _SQL_Connection->prepare("get_invite_details", 
        "SELECT si.server_id, s.name FROM server_invites si JOIN servers s ON si.server_id = s.id WHERE si.token = $1 AND si.expires_at > CURRENT_TIMESTAMP;");

    _SQL_Connection->prepare("check_is_server_member", "SELECT 1 FROM server_members WHERE server_id = $1 AND user_id = $2;");

    _SQL_Connection->prepare("get_user_servers_UUIDs", 
        "SELECT server_id FROM server_members WHERE user_id = $1;");

    _SQL_Connection->prepare("load_server_passport",
        "SELECT name, icon_url FROM servers WHERE id = $1 LIMIT 1;");

    _SQL_Connection->prepare("get_server_channels_UUIDs",
        "SELECT id FROM channels WHERE server_id = $1 ORDER BY created_at ASC;");

    _SQL_Connection->prepare("get_server_roles_data",
        "SELECT id, name, color, permissions, position FROM server_roles WHERE server_id = $1 ORDER BY position ASC;");

    _SQL_Connection->prepare(
        "get_server_members_short",
        "SELECT user_id, role_id FROM server_members WHERE server_id = $1;"
    );
    _SQL_Connection->prepare("get_user_relations", 
        R"( SELECT CASE WHEN user_id = $1 THEN friend_id ELSE user_id END AS related_user_id, status, CASE WHEN user_id = $1 THEN true 
            ELSE false END AS is_owner_sender FROM friends WHERE user_id = $1 OR friend_id = $1;)");

    _SQL_Connection->prepare("load_user_passport",
        "SELECT display_name, avatar_url, status FROM users WHERE id = $1 LIMIT 1;");

    _SQL_Connection->prepare("get_user_joined_servers",
        "SELECT server_id FROM server_members WHERE user_id = $1;");
}

#pragma region CORE {

bool Repository::Run()
{
    bool Result = true;
    try
    {
        //_SQL_Connection = std::make_unique<pqxx::connection>("host=127.0.0.1 dbname=VOX_db user=nomad_admin password=12345678 port=5432");
        _SQL_Connection = std::make_unique<pqxx::connection>("host=127.0.0.1 dbname=VOX_db user=postgres password=12345678 port=5432");

        if (!_SQL_Connection || !_SQL_Connection->is_open()) {
            cout << "REPOSITORY ERROR | Ошибка подключения к PostgreSQL" << endl;
            return false;
        }
        cout << "REPOSITORY | Успешное подключение к СУБД PostgreSQL. Регистрация запросов..." << endl;

        vector<CheckDBrule*> rules;
        rules.push_back(new UsersCheckRule());
        rules.push_back(new CheckServersRule());
        rules.push_back(new CheckSessionsRule());
        rules.push_back(new CheckChannelsRule());
        rules.push_back(new CheckServerRolesRule());
        rules.push_back(new CheckServerInvitesRule());
        rules.push_back(new CheckChannelMembersRule());
        rules.push_back(new CheckMessagesRule());
        rules.push_back(new CheckFriendsRule());
        rules.push_back(new CheckAppVersionsRule());
        rules.push_back(new CheckServerMembersRule());
        rules.push_back(new CheckVoiceStatesRule());
        for (CheckDBrule* rule : rules)
        {
            if (!Result)
            {
                delete rule; continue;
            }

            Result &= rule->CheckTable(_SQL_Connection);

            delete rule;
        }
        if (Result) cout << "REPOSITORY | PostgreSQL: Все таблицы успешно верифицированы!" << endl;

    }
    catch (const std::exception& e) {
        cout << "REPOSITORY CRITICAL EXCEPTION | " << e.what() << endl;
        return false;
    }

    if (!Result)
    {
        if (!Result) cout << "REPOSITORY ERROR | Критическая ошибка инициализации одной из таблиц!" << endl;
        return false;
    }
    try
    {
        cout << "REPOSITORY | Инициализация подготовленных запросов" << endl;
        PrepareQueries();
        cout << "REPOSITORY | Все запросы успешно подготовленны" << endl;
    }
    catch (const std::exception& e) {
        cout << "REPOSITORY CRITICAL EXCEPTION | " << e.what() << endl;
        return false;
    }
    return Result;
}

bool Repository::loadAppVersionData(SAppVersionData& platform)
{
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(
            pqxx::prepped("get_latest_version"),
            pqxx::params{ toLowerTrimmed(platform.Platform) }
        );
        if (!res.empty()) {
            platform.LatestVersionCode = res[0]["version_code"].as<int>();
            platform.LatestVersionName = res[0]["version_name"].as<string>();
            platform.DownloadUrl = res[0]["download_url"].as<string>();
            platform.Sha256Hash = res[0]["sha256_hash"].as<string>();
            return true;
        }
    }
    catch (...) {}
    return false;
}

string Repository::CheckAuth(const SAuthorizationData& Data)
{
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec( pqxx::prepped("check_auth"), pqxx::params{ toLowerTrimmed(Data.Login) }
        );
        if (res.empty()) return "";
        string dbHash = res[0]["password_hash"].as<string>();
        string dbSaltBase64 = res[0]["salt"].as<string>();

        ByteArray Ar = ToolLib::decodeBase64(dbSaltBase64);
        string computedHash = ToolLib::toBase64(CryptoLib::hashPassword(Data.Password, Ar));
        if (computedHash != dbHash) return "";
        return res[0]["id"].as<string>();
    }
    catch (...) {}
    return "";
}
string Repository::RegisterNewAccount(const SAuthorizationData& Data)
{
    string userUUID = "";
    string lowerLogin = toLowerTrimmed(Data.Login);
    if (HasAccount(lowerLogin)) return userUUID;
    try {
        pqxx::work tx(*_SQL_Connection);

        auto SaltByte   = CryptoLib::generateSalt(); 
        string Salt     = ToolLib::toBase64(SaltByte);
        string HashPass = ToolLib::toBase64(CryptoLib::hashPassword(Data.Password, SaltByte));
        string userEmail = Data.Email.empty() ? (lowerLogin + "@example.com") : Data.Email;

        pqxx::result res = tx.exec(pqxx::prepped("insert_user"), pqxx::params{ lowerLogin, Data.Login, userEmail, HashPass, Salt });
        tx.commit();
        if (!res.empty()) {
            userUUID = res[0][0].as<string>();
        }
    }
    catch (const std::exception& e) {
        cout << "REPOSITORY ERROR | PostgreSQL INSERT error: " << e.what() << endl;
    }
    return userUUID;
}
bool Repository::HasAccount(const string& _Username)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("has_account"), pqxx::params{ _Username });
        if (!res.empty()) return res[0][0].as<bool>();
    }
    catch (...) {}
    return true;
}

bool Repository::searchUsersByUsername(const string& myUUID, SSearchUsersData& searchQuery)
{
    searchQuery.Results.clear();
    string queryTrimmed = toLowerTrimmed(searchQuery.SearchQuery);
    if (queryTrimmed.empty()) return false;
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("search_users"), pqxx::params{ queryTrimmed, myUUID });
        for (auto row : res) 
        {
            searchQuery.Results.push_back(row["id"].as<string>());
        }
    }
    catch (...) 
    {
        return false;
    }
    return true;
}


pair<bool, string> Repository::sendFriendRequest(const string& senderUUID, const string& targetUsername)
{
    pair<bool, string> result(false, "");
    try 
    {
        pqxx::work tx(*_SQL_Connection);

        // 1. Ищем пользователя
        pqxx::result res = tx.exec(pqxx::prepped("find_user_by_name"), pqxx::params{ targetUsername });
        if (res.empty()) 
        {
            result.second = "User not found"; // Возвращаем понятную причину
            return result;
        }

        string targetUUID = res[0][0].as<string>();

        if (senderUUID == targetUUID) 
        {
            result.second = "Cannot add self as friend";
            return result;
        }

        pqxx::result checkRes = tx.exec(pqxx::prepped("check_friendship_exists"), pqxx::params{ senderUUID, targetUUID });
        if (!checkRes.empty())
        {
            result.second = "Already friends or request exists";
            return result;
        }

        // 4. Вставка
        tx.exec(pqxx::prepped("insert_friend_request"), pqxx::params{ senderUUID, targetUUID });
        tx.commit();

        result.first = true;
        return result;
    }
    catch (const pqxx::sql_error& e) 
    {
        // Ловим ошибки SQL конкретно
        spdlog::error("[DB] SQL Error in addFriendRequest: {}", e.what());
        result.second = "Database SQL Error: " + string(e.what());
        return result;
    }
    catch (const std::exception& e) {
        // Ловим общие ошибки (сеть, таймауты)
        spdlog::error("[DB] General Error in addFriendRequest: {}", e.what());
        result.second = "Internal Server Error: " + string(e.what());
        return result;
    }
}
bool Repository::handleFriendResponse(const string& myUUID, const string& targetUUID, int action)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res;
        if (action == 0) 
        {
            pqxx::result res = tx.exec(pqxx::prepped("accept_friend"), pqxx::params{ targetUUID, myUUID });
        }
        else 
        {
            pqxx::result res = tx.exec(pqxx::prepped("delete_friend"), pqxx::params{ myUUID, targetUUID });
        }
        tx.commit();
        return res.affected_rows() > 0;
    }
    catch (...) {}
    return false;
}

bool Repository::getUserRelations(const string& myUUID, SUserRelations& Result)
{
    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_user_relations"), pqxx::params{ myUUID });

        Result.friendsUUID.clear();
        Result.IncRequestFriend.clear();
        Result.OutcRequestFrined.clear();

        for (auto row : res)
        {
            string relatedUUID = row["related_user_id"].as<string>();
            string status = row["status"].as<string>();
            bool isOwnerSender = row["is_owner_sender"].as<bool>();

            if (status == "accepted")
            {
                Result.friendsUUID.insert(relatedUUID);
            }
            else if (status == "pending")
            {
                if (isOwnerSender)
                {
                    Result.OutcRequestFrined.insert(relatedUUID);
                }
                else
                {
                    Result.IncRequestFriend.insert(relatedUUID);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error("[DB] Error in getUserRelations for {}: {}", myUUID, e.what());
        return false;
    }

    return true;
}

bool Repository::loadUserPassport(SUserPassport& Result)
{
    if (Result.UUID.empty()) return false;

    try
    {
        pqxx::work tx(*_SQL_Connection);

        pqxx::result userRes = tx.exec(
            pqxx::prepped("load_user_passport"),
            pqxx::params{ Result.UUID }
        );

        if (userRes.empty())
        {
            spdlog::warn("[DB] User passport not found for UUID: {}", Result.UUID);
            return false;
        }

        auto row = userRes[0];
        Result.DisplayName = row["display_name"].as<string>();
        Result.AvatarUrl = row["avatar_url"].is_null() ? "" : row["avatar_url"].as<string>();
        int rawStatus = row["status"].as<int>();
        Result.PresenceStatus = rawStatus;

        pqxx::result serversRes = tx.exec(
            pqxx::prepped("get_user_joined_servers"),
            pqxx::params{ Result.UUID }
        );

        Result.joinedServersUUID.clear();
        for (auto serverRow : serversRes)
        {
            if (!serverRow["server_id"].is_null())
            {
                Result.joinedServersUUID.insert(serverRow["server_id"].as<string>());
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        spdlog::error("[DB] Error in loadUserPassport for UUID {}: {}", Result.UUID, e.what());
    }
    return false;
}

string Repository::findPrivateChannel(const string& senderUUID, const string& targetUUID)
{
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("find_private_channel"), pqxx::params{ senderUUID, targetUUID });

        if (!res.empty()) return res[0][0].as<string>();
    }
    catch (...) {}
    return "";
}
bool Repository::createPrivateChannel(const string& roomUUID, const string& senderUUID, const string& targetUUID)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        tx.exec(pqxx::prepped("insert_channel"), pqxx::params{ roomUUID });
        tx.exec(pqxx::prepped("insert_channel_member"), pqxx::params{ roomUUID, senderUUID });
        tx.exec(pqxx::prepped("insert_channel_member"), pqxx::params{ roomUUID, targetUUID });
        tx.commit();
        return true;
    }
    catch (...) {
        return false;
    }
}
string Repository::getOrCreatePrivateChannel(const string& senderUUID, const string& targetUUID)
{
    string existing = findPrivateChannel(senderUUID, targetUUID);
    if (!existing.empty()) return existing;
    string newRoomUUID = generateUUID();
    if (createPrivateChannel(newRoomUUID, senderUUID, targetUUID)) return newRoomUUID;
    return "";
}
vector<SChannelPassport> Repository::getUserDirectChats(const string& userUUID)
{
    vector<SChannelPassport> chatList;
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_user_direct_chats"), pqxx::params{ userUUID });
        

        for (auto row : res) 
        {
            SChannelPassport chat;
            chat.ChatUUID = row["id"].as<string>();
            chat.ChatType = row["type"].as<int>();
            chat.Name = row["name"].is_null() ? "" : row["name"].as<string>();
            pqxx::result parts = tx.exec(pqxx::prepped("get_channel_participants"), pqxx::params{ chat.ChatUUID });

            for (auto p_row : parts) 
            {
                chat.ParticipantUUIDs.push_back(p_row[0].as<string>());
            }

            pqxx::result msg = tx.exec(pqxx::prepped("get_last_channel_message"), pqxx::params{ chat.ChatUUID });
            if (!msg.empty()) 
            {
                chat.Description = msg[0][0].as<string>();
            }
            chatList.push_back(chat);
        }
    }
    catch (...) {}
    return chatList;
}
bool Repository::createServerChannelInDB(const string& channelUUID, const string& serverUUID, const string& name, int chatType)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        tx.exec(pqxx::prepped("insert_server_channel"), pqxx::params{ channelUUID, chatType, serverUUID, name });
        tx.commit();
        return true;
    }
    catch (...) {
        return false;
    }
}
bool Repository::saveMessage(const SMessageData& msg)
{
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("insert_message"), pqxx::params{ msg.RoomUUID, msg.SenderUUID, msg.Message });

        if (res.empty()) return false;
        string generatedMessageId = res[0][0].as<string>();

        if (!msg.Attachments.empty()) 
        {
            for (const auto& attach : msg.Attachments) 
            {
                tx.exec(pqxx::prepped("insert_attachment"), pqxx::params{ generatedMessageId, static_cast<int>(attach.Type), attach.Url, attach.MetaData });
            }
        }

        tx.commit();
        return true;
    }
    catch (const std::exception& e) {
        cout << "REPOSITORY ERROR | Не удалось сохранить сообщение: " << e.what() << endl;
        return false;
    }
}
vector<SMessageData> Repository::loadHistoryFromDB(const string& roomUUID, int limit, int offset)
{
    vector<SMessageData> history;
    if (roomUUID.empty()) return history;
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("load_history"), pqxx::params{ roomUUID, limit, offset });
        vector<string> loadedMessageUUIDs;
        unordered_map<string, size_t> messageIdToHistoryIndex;
        for (auto row : res) 
        {
            string msgId = row["id"].as<string>();
            string senderId = row["sender_id"].as<string>();
            string text = row["message_text"].as<string>();
            SMessageData msg(roomUUID, senderId, text);
            history.push_back(msg);
            loadedMessageUUIDs.push_back(msgId);
            messageIdToHistoryIndex[msgId] = history.size() - 1;
        }
        if (!loadedMessageUUIDs.empty()) {
            std::stringstream ss;
            ss << "SELECT message_id, attachment_type, url, meta_data FROM message_attachments WHERE message_id IN (";
            for (size_t i = 0; i < loadedMessageUUIDs.size(); ++i) {
                ss << tx.quote(loadedMessageUUIDs[i]) << (i == loadedMessageUUIDs.size() - 1 ? "" : ",");
            }
            ss << ");";
            pqxx::result attachRes = tx.exec(ss.str());
            for (auto row : attachRes) {
                string targetMsgId = row["message_id"].as<string>();
                SAttachmentData attach;
                attach.Type = static_cast<EAttachmentType>(row["attachment_type"].as<int>());
                attach.Url = row["url"].as<string>();
                attach.MetaData = row["meta_data"].as<string>();
                if (messageIdToHistoryIndex.find(targetMsgId) != messageIdToHistoryIndex.end()) {
                    size_t idx = messageIdToHistoryIndex[targetMsgId];
                    history[idx].Attachments.push_back(attach);
                }
            }
        }
    }
    catch (...) {}
    std::reverse(history.begin(), history.end());
    return history;
}
bool Repository::isChannelExists(const string& roomUUID)
{
    if (roomUUID.empty()) return false;
    try {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("is_channel_exists"), pqxx::params{ roomUUID });
        return !res.empty();
    }
    catch (...) {}
    return false;
}
bool Repository::generateAndSaveSession(const string& userUUID, const string& deviceInfo, string& resultUUID)
{
    resultUUID = string("");
    if (userUUID.empty()) return false;
    string token = generateUUID();
    string expiresAt = getDateTimeOffsetInDays(30);
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        tx.exec(pqxx::prepped("insert_session"), pqxx::params{ userUUID, token, deviceInfo, expiresAt });
        tx.commit();
        resultUUID = token;
        return true;
    }
    catch (...) {
        return false;
    }
}
bool Repository::validateRefreshToken(const string& token, const string& platform, string& resultUUID)
{
    resultUUID = string("");
    if (token.empty()) return false;

    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("validate_session"), pqxx::params{ token });
        if (res.empty()) return "";
        string expiresAtStr = res[0]["expires_at"].as<string>();
        if (isExpired(expiresAtStr)) {
            tx.exec(pqxx::prepped("delete_session"), pqxx::params{ token });
            tx.commit();
            return "";
        }
        resultUUID = res[0]["user_id"].as<string>();
        tx.exec(pqxx::prepped("update_session_platform"), pqxx::params{ platform, token });
        tx.commit();
        return true;
    }
    catch (...) {}
    return false;
}
void Repository::deleteSessionByToken(const string& token)
{
    if (token.empty()) return;
    try {
        pqxx::work tx(*_SQL_Connection);
        tx.exec(pqxx::prepped("delete_session"), pqxx::params{ token });
        tx.commit();
    }
    catch (...) {}
}

bool Repository::createServerInDB(const SMakeServerTask& result_MakeTask, SServerPassport& Result)
{
    Result = SServerPassport();
    try
    {
        pqxx::work tx(*_SQL_Connection);

        // 1. Выполняем вставку сервера и получаем возвращаемые базой данные
        pqxx::result res = tx.exec(pqxx::prepped("insert_server"), pqxx::params{ result_MakeTask.Name, result_MakeTask.userUUID });

        if (!res.empty())
        {
            Result.ServerUUID = res[0]["id"].as<string>();
            Result.Name = res[0]["name"].as<string>();
            Result.IconUrl = res[0]["icon_url"].as<string>();
        }
        else
        {
            if (res.empty()) { throw std::runtime_error("Database returned no rows for insert_server"); }
        }
        tx.exec(pqxx::prepped("insert_server_member"), pqxx::params{ Result.ServerUUID, result_MakeTask.userUUID });
        tx.commit();
        return true;
    }
    catch (const pqxx::sql_error& e)
    {
        cerr << "SQL Error in createServerInDB: " << e.what() << endl;
        return false; // Возвращаем пустой паспорт в случае ошибки
    }
    catch (const std::exception& e)
    {
        cerr << "Error in createServerInDB: " << e.what() << endl;
        return false;
    }
}

bool Repository::createInviteInDB(const string& token, const string& serverUUID, const string& inviterUUID)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        tx.exec(pqxx::prepped("insert_invite"), pqxx::params{ token, serverUUID, inviterUUID });
        tx.commit();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool Repository::updateVoiceStateInDB(const string& userUUID, const string& serverUUID, const string& channelUUID)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        std::optional sUUID = serverUUID.empty() ? std::nullopt : std::make_optional(serverUUID);
        tx.exec(pqxx::prepped("upsert_voice_state"), pqxx::params{ userUUID, sUUID, channelUUID });
        tx.commit();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool Repository::removeVoiceStateFromDB(const string& userUUID)
{
    try {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("delete_voice_state"), pqxx::params{ userUUID });
        tx.commit();
        return res.affected_rows() > 0;
    }
    catch (...) {
        return false;
    }
}

unordered_map<string, vector<string>> Repository::getServerVoiceStates(const string& serverUUID)
{
    unordered_map<string, vector<string>> states;
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_server_voice_states"), pqxx::params{ serverUUID });
        for (auto row : res) 
        {
            string chID = row["channel_id"].as<string>();
            string uID = row["user_id"].as<string>();
            states[chID].push_back(uID);
        }
    }
    catch (...) {}
    return states;
}

bool Repository::acceptInviteInDB(const string& token, const string& userUUID)
{
    try 
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_invite_details"), pqxx::params{ token });
        if (res.empty()) return false;

        string serverUUID = res[0]["server_id"].as<string>();
        pqxx::result checkMember = tx.exec(pqxx::prepped("check_is_server_member"), pqxx::params{ serverUUID, userUUID });
        if (!checkMember.empty()) return false;

        tx.exec(pqxx::prepped("insert_server_member"), pqxx::params{ serverUUID, userUUID });
        tx.commit();
        return true;
    }
    catch (...) {}
    return false;
}

vector<string> Repository::getUserServersUUIDs(const string& userUUID)
{
    vector<string> UUIDs;
    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_user_servers_UUIDs"), pqxx::params{ userUUID });

        for (auto row : res)
        {
            UUIDs.push_back(row["server_id"].as<string>());
        }
    }
    catch (...) {}

    return UUIDs;
}

bool Repository::loadServerPassport(SServerPassport& _ServerPasport)
{
    if (_ServerPasport.ServerUUID.empty()) return false;

    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("load_server_passport"), pqxx::params{ _ServerPasport.ServerUUID });

        if (!res.empty())
        {
            _ServerPasport.Name = res[0]["name"].as<string>();
            // Защита от NULL в базе для icon_url
            _ServerPasport.IconUrl = res[0]["icon_url"].is_null() ? "" : res[0]["icon_url"].as<string>();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        cerr << "REPOSITORY ERROR | loadServerPassport failed: " << e.what() << endl;
    }
    return false; 
}

bool Repository::loadServerChannelsUUIDs(const string& serverUUID, vector<string>& _Result)
{
    if (serverUUID.empty()) return false;

    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_server_channels_UUIDs"), pqxx::params{ serverUUID });

        _Result.reserve(res.size());
        for (auto row : res)
        {
            _Result.push_back(row["id"].as<string>());
        }
    }
    catch (...) {}
    return true;
}

bool Repository::loadServerRoles(const string& serverUUID, vector<SServerRoleData>& _Result)
{
    if (serverUUID.empty()) return false;

    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_server_roles_data"), pqxx::params{ serverUUID });

        _Result.reserve(res.size());
        for (auto row : res)
        {
            SServerRoleData role;
            role.RoleUUID = row["id"].as<string>();
            role.Name = row["name"].as<string>();
            role.Color = row["color"].is_null() ? "#99aab5" : row["color"].as<string>();
            role.Permissions = row["permissions"].as<int64_t>();
            role.Position = row["position"].as<int16_t>();
            _Result.push_back(role);
        }
    }
    catch (...) {}
    return true;
}

bool Repository::loadServerMembersWithRoles(const string& serverUUID, vector<SServerMemberShortData>& _Result)
{
    if (serverUUID.empty()) return false;

    try
    {
        pqxx::work tx(*_SQL_Connection);
        pqxx::result res = tx.exec(pqxx::prepped("get_server_members_short"), pqxx::params{ serverUUID });
        _Result.reserve(res.size());
        for (auto row : res)
        {
            SServerMemberShortData member;
            member.UserUUID = row["user_id"].as<string>();
            member.RoleUUID = row["role_id"].as<string>(""); // Если в базе NULL -> ""

            _Result.push_back(member);
        }
    }
    catch (...) {}
    return true;
}