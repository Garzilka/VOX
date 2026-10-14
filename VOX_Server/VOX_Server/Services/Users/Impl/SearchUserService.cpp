#include "SearchUserService.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"

void SearchUserServiceImpl::handleSearchUsers(const string& responseClientUUID, const SSearchUsersData& data)
{
    if (data.SearchQuery.empty()) return;
    if (responseClientUUID.empty()) return;

    SSearchUserBD requestData;
    requestData.userUUID = responseClientUUID;
    requestData._SearchUsersData = data;

    BDTask newTask(EBDTaskType::SearchUser);
    newTask.payload = requestData.ToJSON();
    std::weak_ptr<SearchUserServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]()
                    {
                        self->responseFromDB(res);
                    });
            }
        };

    m_bdQueue.push(std::move(newTask));
}

void SearchUserServiceImpl::responseFromDB(const JsonObject& obj)
{
    SSearchUserBD requestData(obj);
    if (requestData.userUUID.empty()) return;

    notificationService->NotifyClients({ requestData.userUUID }, requestData._SearchUsersData);
}
