#pragma once


#include "../UserProfileInterfaces.h"

class BDTaskQueue;
class INotificationService;

class SearchUserServiceImpl : public ISearchUserService,
    public std::enable_shared_from_this<SearchUserServiceImpl>
{
    shared_ptr<INotificationService> notificationService;
    mutable mutex _Mutex;

    asio::io_context& m_ioContext;
    BDTaskQueue& m_bdQueue;
public:
    SearchUserServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue, shared_ptr<INotificationService> in_NotificationService)
        : m_ioContext(ioCtx), m_bdQueue(bdQueue), notificationService(in_NotificationService) {
    }
    void handleSearchUsers(const string& responseClientUUID, const SSearchUsersData& data) override;
private:
    void responseFromDB(const JsonObject& obj);
};