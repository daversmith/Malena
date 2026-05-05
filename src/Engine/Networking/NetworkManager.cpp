// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Engine/Networking/NetworkManager.h>
#include <Malena/Engine/App/AppManager.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>

namespace ml
{
    // ── Internal state ────────────────────────────────────────────────────────

    namespace
    {
        // ── Background thread pool ────────────────────────────────────────────

        std::vector<std::thread>          gWorkers;
        std::queue<std::function<void()>> gWorkQueue;
        std::mutex                        gWorkMutex;
        std::condition_variable           gWorkCv;
        std::atomic<bool>                 gShutdown{ false };
        std::atomic<bool>                 gInitialised{ false };

        // ── Main-thread callback queue ────────────────────────────────────────

        std::queue<std::function<void()>> gCallbackQueue;
        std::mutex                        gCallbackMutex;

        // ── Worker loop ───────────────────────────────────────────────────────

        void workerLoop()
        {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(gWorkMutex);
                    gWorkCv.wait(lock, []{ return gShutdown || !gWorkQueue.empty(); });

                    if (gShutdown && gWorkQueue.empty()) return;

                    task = std::move(gWorkQueue.front());
                    gWorkQueue.pop();
                }
                task();
            }
        }
    } // anonymous namespace

    // ── NetworkManager implementation ─────────────────────────────────────────

    void NetworkManager::initialize(std::size_t threadCount)
    {
        if (gInitialised.exchange(true)) return; // already running

        gShutdown = false;
        gWorkers.reserve(threadCount);
        for (std::size_t i = 0; i < threadCount; ++i)
            gWorkers.emplace_back(workerLoop);
    }

    void NetworkManager::shutdown()
    {
        if (!gInitialised) return;

        {
            std::lock_guard<std::mutex> lock(gWorkMutex);
            gShutdown = true;
        }
        gWorkCv.notify_all();

        for (auto& t : gWorkers)
            if (t.joinable()) t.join();

        gWorkers.clear();
        gInitialised = false;
    }

    void NetworkManager::autoFlush()
    {
        AppManager::get().onPreRender([]{ NetworkManager::flush(); });
    }

    void NetworkManager::flush()
    {
        std::queue<std::function<void()>> local;
        {
            std::lock_guard<std::mutex> lock(gCallbackMutex);
            std::swap(local, gCallbackQueue);
        }
        while (!local.empty())
        {
            local.front()();
            local.pop();
        }
    }

    void NetworkManager::enqueue(std::function<void()> work,
                                  std::function<void()> onComplete)
    {
        if (!gInitialised)
            initialize();

        {
            std::lock_guard<std::mutex> lock(gWorkMutex);
            gWorkQueue.push([w = std::move(work), cb = std::move(onComplete)]() mutable
            {
                w();
                NetworkManager::queueCallback(std::move(cb));
            });
        }
        gWorkCv.notify_one();
    }

    void NetworkManager::queueCallback(std::function<void()> callback)
    {
        std::lock_guard<std::mutex> lock(gCallbackMutex);
        gCallbackQueue.push(std::move(callback));
    }

} // namespace ml
