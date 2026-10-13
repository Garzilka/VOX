#pragma once

#include "Common.h"
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/use_awaitable.hpp>

using TCPSoket = asio::ip::tcp::socket;
using acceptor = asio::ip::tcp::acceptor;
using steady_timer = asio::steady_timer;


class AsioThreadPool
{
public:
    explicit AsioThreadPool(asio::io_context& result_ioContext, size_t threadsCount = 4)
        : m_threadsCount(threadsCount)
        , m_work(m_ioContext.get_executor())
        , m_ioContext(result_ioContext) {
    }

    asio::io_context& getIoContext() { return m_ioContext; }

    void start()
    {
        for (size_t i = 0; i < m_threadsCount; ++i)
        {
            m_threads.emplace_back([this]()
            {
                try
                {
                    m_ioContext.run();
                }
                catch (const std::exception& e)
                {
                    std::cout << "CHAT THREADS: " << e.what() << "\n";
                }
            });
        }
    }

    void stop()
    {
        m_ioContext.stop();

        for (auto& t : m_threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    void restart()
    {
        if (!m_ioContext.stopped())
        {
            return;
        }
        m_ioContext.restart();
        start();
    }

private:
    asio::io_context& m_ioContext;
    asio::executor_work_guard<asio::io_context::executor_type> m_work;
    size_t m_threadsCount;
    std::vector<std::thread> m_threads;
};