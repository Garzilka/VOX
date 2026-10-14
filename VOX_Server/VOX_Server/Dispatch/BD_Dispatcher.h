#pragma once
#include "BD_Commands/BD_Commands.h"
#include "Data/BDTaskTypes.h"

class BDWorker;

class BD_Dispatcher
{
    unordered_map<EBDTaskType, shared_ptr<IDBCommandHandler>> m_Commands;

public:
    explicit BD_Dispatcher(const shared_ptr<Repository>& result_Repository);
    void dispatch(const BDTask& data)
    {
        auto it = m_Commands.find(data.type);
        if (it != m_Commands.end())
        {
            it->second->execute(data);
        }
        else
        {
            REPOSITORY_WARNING("[Dispatcher] Неизвестный тип сообщения: {}", static_cast<int>(data.type));
        }
        return;
    }
private:
    void registerHandler(EBDTaskType type, shared_ptr<IDBCommandHandler> handler)
    {
        m_Commands.insert({ type, handler });
    }
};