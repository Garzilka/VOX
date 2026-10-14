#pragma once
#include "../RelationshipInterfaces.h"
#include "Data/Common.h"
#include "Data/AsioThreads.h"


class BDTaskQueue;
class Relation;
class INotificationService;

class RelationCacheServiceImpl : public IRelationCacheService,
    public std::enable_shared_from_this<RelationCacheServiceImpl>
{

    unordered_map<string, std::shared_ptr<Relation>> m_readyPresence;
    unordered_map<string, std::shared_ptr<Relation>> m_wakeupPresence;
    shared_ptr<INotificationService> notificationService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;

public:
    RelationCacheServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<INotificationService> in_NotificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(in_NotificationService) {
    }

    virtual bool handleUserRelations(const string& responseClientUUID, SUserRelations& outRelations) override;
    virtual void warmup(std::function<void(string userUUID)> Success, string userUUID) override;

private:
    std::shared_ptr<Relation> findOrAddPresence(const string& userUUID);
    void requestPresenceInDB(const string& userUUID);
    void responsePresenceFromDB(const JsonObject& obj);
    virtual void addNewFriend(const string& responseClientUUID, const string& targetClientUUID) override;
    virtual void addRequestFriend(const string& responseClientUUID, const string& targetClientUUID) override;
    virtual void removeFriend(const string& responseClientUUID, const string& targetClientUUID) override;
    virtual void removeRequestFriend(const string& responseClientUUID, const string& targetClientUUID) override;
    bool loadComplete(const string& userUUID);
    std::shared_ptr<Relation> takePresence(unordered_map<string, std::shared_ptr<Relation>>& map, const string& userUUID);
};