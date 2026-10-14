#pragma once
#include "../UserProfileInterfaces.h"
#include "../User.h"

class BDTaskQueue;
class INotificationService;

class CacheUserServiceImpl : public IUserProfileCacheService,
    public std::enable_shared_from_this<CacheUserServiceImpl>
{
    unordered_map<string, shared_ptr<User>> m_readyUser;
    unordered_map<string, shared_ptr<User>> m_wakeupUser;
    shared_ptr<INotificationService> notificationService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;
public:
    CacheUserServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<INotificationService> in_NotificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(in_NotificationService) {}

    virtual bool getUserPassport(const string& responseClientUUID, SUserPassport& newPassport) override;

    virtual void warmup(std::function<void(string userUUID)> Success, string userUUID) override;
private:
    shared_ptr<User> findOrAddUser(const string& socketID);
    void requestPassportInDB(const string& userUUID);
    void responsePassportFromDB(const JsonObject& obj);
    bool loadComplete(const string& userUUID);
    shared_ptr<User> takeUser(unordered_map<string, shared_ptr<User>>& map, const string& userUUID);
};