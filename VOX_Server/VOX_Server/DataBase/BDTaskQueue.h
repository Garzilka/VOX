#pragma once
#include "Data/BDTaskTypes.h"



class BDTaskQueue
{
    queue<BDTask> m_queue;
    mutable mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stopped{ false };

public:
    void push(BDTask task)
    {
        std::lock_guard<mutex> lock(m_mutex);
        m_queue.push(std::move(task));
        m_condition.notify_one();
    }

    bool pop(BDTask& outTask)
    {
        std::unique_lock<mutex> lock(m_mutex);

        m_condition.wait(lock, [this]() 
            {
            return !m_queue.empty() || m_stopped;
            });

        if (m_stopped && m_queue.empty()) 
        {
            return false; // Пора выключаться
        }

        outTask = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // Завершение работы пула
    void stop() 
    {
        std::lock_guard<mutex> lock(m_mutex);
        m_stopped = true;
        m_condition.notify_all();
    }
};