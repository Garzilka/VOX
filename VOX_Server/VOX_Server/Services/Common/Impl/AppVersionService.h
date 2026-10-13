#pragma once

#include "Data/Common.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../../Data/AsioThreads.h"
#include "../CommonInterfaces.h"


class AppVersionServiceImpl : public IAppVersionService,
    public std::enable_shared_from_this<AppVersionServiceImpl>
{
    mutable std::mutex m_mutex;

    asio::io_context& m_ioContext;
    asio::steady_timer m_refreshTimer;
    BDTaskQueue& m_bdQueue;

    SAppVersionData _AppVersion;

public:
    AppVersionServiceImpl(asio::io_context& ioCtx, BDTaskQueue& bdQueue)
        : m_ioContext(ioCtx), m_refreshTimer(ioCtx), m_bdQueue(bdQueue)
    {
        _AppVersion.Platform = "windows";
    };

    void start()
    {
        refreshVersionBackground();
    }

    virtual void handleAppVersionRequest(shared_ptr<Client> client, const JsonObject& data) override;

private:

    void refreshVersionBackground()
    {
        /*BDTask newTask(EBDTaskType::LoadAppVersion);
        newTask.payload = _AppVersion.ToJSON();
        std::weak_ptr<AppVersionServiceImpl> weakSelf = shared_from_this();

        newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]() 
                {
                    self->dbResponse(res);
                    self->setupNextRefresh();
                });
            }
        };

        m_bdQueue.push(std::move(newTask));*/
    }
    void setupNextRefresh()
    {
        /*m_refreshTimer.expires_after(std::chrono::minutes(5));

        std::weak_ptr<AppVersionServiceImpl> weakSelf = shared_from_this();
        m_refreshTimer.async_wait([weakSelf](const asio::error_code& ec)
        {
            if (ec == asio::error::operation_aborted) return;

            if (auto self = weakSelf.lock()) 
            {
                self->refreshVersionBackground(); 
            }
        });*/
    }
    void dbResponse(const JsonObject& res)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        _AppVersion = SAppVersionData(res);
    }
};