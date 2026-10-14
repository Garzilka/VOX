#include "CacheUserServiceImpl.h"
#include "Data/BDTaskTypes.h"
#include "Data/AsioThreads.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"


bool CacheUserServiceImpl::getUserPassport(const string& responseClientUUID, SUserPassport& newPassport)
{
    if (newPassport.UUID.empty())
    {
        USER_WARNING("Requested UUID as null");
        return false;
    }

    if (responseClientUUID.empty())
    {
        USER_WARNING("Requester as null");
        return false;
    }

    auto User = findOrAddUser(newPassport.UUID);
    if (!User)
    {
        USER_ERROR("No maked user");
        return false;
    }

    SUserPassport ServerPas;
    if (!User->getUserPassport(ServerPas, responseClientUUID))
    {
        USER_INFO("No cached passport for: {}, add pendings list", responseClientUUID);
        return false;
    }

    return true;
}

void CacheUserServiceImpl::requestPassportInDB(const string& userUUID)
{
    if (userUUID.empty()) return;

    USER_INFO("Request in DB for: {}", userUUID);
    SUserPassport Pas;
    Pas.UUID = userUUID;

    BDTask newTask(EBDTaskType::LoadUserPassport);
    newTask.payload = Pas.ToJSON();
    std::weak_ptr<CacheUserServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]()
                    {
                        self->responsePassportFromDB(res);
                    });
            }
        };

    m_bdQueue.push(std::move(newTask));
}

void CacheUserServiceImpl::responsePassportFromDB(const JsonObject& obj)
{
    SUserPassport ServerPas(obj);
    shared_ptr<User> wakeupUser = nullptr;


    if (!ServerPas.UUID.empty())
    {
        USER_INFO("Response from DB for: {}", ServerPas.UUID);
    }
    else
    {
        USER_WARNING("Bad response from DB for: {}", ServerPas.UUID);
        return;
    }

    {
        std::lock_guard<mutex> lock(_Mutex);

        shared_ptr<User> wakeupUser = takeUser(m_wakeupUser, ServerPas.UUID);

        if (!wakeupUser)
        {
            USER_WARNING("No wakeup User: {}", ServerPas.UUID);
            return;
        }


        vector<string> usersUUID;
        if (wakeupUser->setUserPassport(ServerPas, usersUUID))  //Remove wakeupUser if false
        {
            m_readyUser.try_emplace(ServerPas.UUID, std::move(wakeupUser));
            if (!usersUUID.empty() && notificationService)
            {
                notificationService->NotifyClients(usersUUID, ServerPas);
            }
        }
    }
}

bool CacheUserServiceImpl::loadComplete(const string& userUUID)
{
    if (userUUID.empty()) return false;

    std::lock_guard<mutex> lock(_Mutex);

    shared_ptr<User> wakeupUser = takeUser(m_wakeupUser, userUUID);
    if (!wakeupUser) return false;

    m_readyUser.try_emplace(std::move(userUUID), std::move(wakeupUser));
    
    return true;
}

shared_ptr<User> CacheUserServiceImpl::takeUser(unordered_map<string, shared_ptr<User>>& map, const string& userUUID)
{
    auto node = map.extract(userUUID);
    if (node.empty()) return nullptr;
    return std::move(node.mapped());
}

shared_ptr<User> CacheUserServiceImpl::findOrAddUser(const string& userUUID)
{
    if (userUUID.empty()) return nullptr;

    std::lock_guard<mutex> lock(_Mutex);

    auto ready = m_readyUser.find(userUUID);
    if (ready != m_readyUser.end()) return ready->second;

    auto wakeup = m_wakeupUser.find(userUUID);
    if (wakeup != m_wakeupUser.end()) return wakeup->second;

    shared_ptr<User> result = std::make_shared<User>();

    m_wakeupUser.try_emplace(userUUID, result);
    requestPassportInDB(userUUID);

    return result;
}

void CacheUserServiceImpl::warmup(std::function<void(string userUUID)> Success, string userUUID)
{
    auto User = findOrAddUser(userUUID);
    if (!User) return;

    User->warmup(Success);
}