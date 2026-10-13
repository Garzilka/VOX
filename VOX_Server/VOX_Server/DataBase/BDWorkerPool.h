#pragma once

#include "BDTaskQueue.h"
#include "Data/BDTaskTypes.h"
#include "Dispatch/BD_Dispatcher.h"


class BDWorkerPool
{
    BDTaskQueue& _Queue;
    shared_ptr<BD_Dispatcher> _Dispatcher;
    vector<std::thread> m_threads;

public:
    explicit BDWorkerPool(size_t threadCount, BDTaskQueue& in_Queue, shared_ptr<BD_Dispatcher> in_Dispatcher) : _Queue(in_Queue), _Dispatcher(in_Dispatcher)
    {
        for (size_t i = 0; i < threadCount; ++i) 
        {
            m_threads.emplace_back(&BDWorkerPool::run, this);
        }
    }

    ~BDWorkerPool()
    {
        stop();
    }

    void stop() 
    {
        _Queue.stop();
        for (auto& t : m_threads) 
        {
            if (t.joinable()) t.join();
        }
    }

private:
    void run()
    {
        BDTask task;
        while (_Queue.pop(task))
        {
            try 
            {
                if (!_Dispatcher) return;
                _Dispatcher->dispatch(task);
            }
            catch (const std::exception& e) 
            {
                JsonObject errorJson;
                errorJson["error"] = e.what();
                if (task.onResult) 
                {
                    task.onResult(errorJson, false);
                }
            }
        }
    }
};