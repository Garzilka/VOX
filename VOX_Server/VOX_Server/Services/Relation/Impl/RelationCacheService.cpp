#include "RelationCacheService.h"
#include "Data/BDTaskTypes.h"
#include "DataBase/BDTaskQueue.h"
#include "../../Common/CommonInterfaces.h"
#include "../Relation.h"

bool RelationCacheServiceImpl::handleUserRelations(const string& responseClientUUID, SUserRelations& outRelations)
{
    if (responseClientUUID.empty()) return false;

    auto presence = findOrAddPresence(responseClientUUID);
    if (!presence) return false;

    SUserRelations serverPresences;
    if (!presence->getUserRelations(serverPresences, responseClientUUID)) return false;

    outRelations = std::move(serverPresences);
    return true;
}

void RelationCacheServiceImpl::warmup(std::function<void(string userUUID)> Success, string userUUID)
{
    auto Relation = findOrAddPresence(userUUID);
    if (!Relation) return;

    Relation->warmup(Success);
}

std::shared_ptr<Relation> RelationCacheServiceImpl::findOrAddPresence(const string& userUUID)
{
    if (userUUID.empty()) return nullptr;

    std::lock_guard<mutex> lock(_Mutex);

    auto ready = m_readyPresence.find(userUUID);
    if (ready != m_readyPresence.end()) return ready->second;

    auto wakeup = m_wakeupPresence.find(userUUID);
    if (wakeup != m_wakeupPresence.end()) return wakeup->second;

    auto result = std::make_shared<Relation>();

    m_wakeupPresence.try_emplace(userUUID, result);
    requestPresenceInDB(userUUID);

    return result;
}

void RelationCacheServiceImpl::requestPresenceInDB(const string& userUUID)
{
    if (userUUID.empty()) return;

    SUserRelations pres;
    pres.userOwnerUUID = userUUID;

    BDTask newTask(EBDTaskType::LoadRelation);
    newTask.payload = pres.ToJSON();
    std::weak_ptr<RelationCacheServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]()
                {
                    self->responsePresenceFromDB(res);
                });
            }
        };

    m_bdQueue.push(std::move(newTask));
}

void RelationCacheServiceImpl::responsePresenceFromDB(const JsonObject& obj)
{
    SUserRelations serverPresences(obj);
    std::shared_ptr<Relation> wakeupPresence = nullptr;

    {
        std::lock_guard<mutex> lock(_Mutex);
        auto it = m_wakeupPresence.find(serverPresences.userOwnerUUID);
        if (it != m_wakeupPresence.end())
        {
            wakeupPresence = it->second;
        }
    }

    if (!wakeupPresence) return;

    vector<string> pendingClientsUUID;
    if (wakeupPresence->setUserRelations(serverPresences, pendingClientsUUID))
    {
        if (loadComplete(serverPresences.userOwnerUUID))
        {
            notificationService->NotifyClients(pendingClientsUUID, serverPresences);
        }
    }
}

void RelationCacheServiceImpl::addNewFriend(const string& responseClientUUID, const string& targetClientUUID)
{
    if (responseClientUUID.empty()) return;
    if (targetClientUUID.empty()) return;

    auto Relation = findOrAddPresence(responseClientUUID);
    if (!Relation) return;
    if (!Relation->isValid()) return;
    Relation->addNewFriend(targetClientUUID);
}

void RelationCacheServiceImpl::addRequestFriend(const string& responseClientUUID, const string& targetClientUUID)
{
    if (responseClientUUID.empty()) return;
    if (targetClientUUID.empty()) return;

    auto Relation = findOrAddPresence(responseClientUUID);
    if (!Relation) return;
    if (!Relation->isValid()) return;
    Relation->addRequestFriend(targetClientUUID);
}

void RelationCacheServiceImpl::removeFriend(const string& responseClientUUID, const string& targetClientUUID)
{
    if (responseClientUUID.empty()) return;
    if (targetClientUUID.empty()) return;

    auto Relation = findOrAddPresence(responseClientUUID);
    if (!Relation) return;
    if (!Relation->isValid()) return;
    Relation->removeFriend(targetClientUUID);
}

void RelationCacheServiceImpl::removeRequestFriend(const string& responseClientUUID, const string& targetClientUUID)
{
    if (responseClientUUID.empty()) return;
    if (targetClientUUID.empty()) return;

    auto Relation = findOrAddPresence(responseClientUUID);
    if (!Relation) return;
    if (!Relation->isValid()) return;
    Relation->removeRequestFriend(targetClientUUID);
}

bool RelationCacheServiceImpl::loadComplete(const string& userUUID)
{
    if (userUUID.empty()) return false;

    std::lock_guard<mutex> lock(_Mutex);

    auto wakeupPresence = takePresence(m_wakeupPresence, userUUID);
    if (!wakeupPresence) return false;

    m_readyPresence.try_emplace(userUUID, std::move(wakeupPresence));

    return true;
}

std::shared_ptr<Relation> RelationCacheServiceImpl::takePresence(unordered_map<string, std::shared_ptr<Relation>>& map, const string& userUUID)
{
    auto node = map.extract(userUUID);
    if (node.empty()) return nullptr;
    return std::move(node.mapped());
}
