// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_NETWORKMANAGER_H
#define MALENA_NETWORKMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <cstddef>
#include <functional>

namespace ml
{
    /**
     * @brief Thread pool and main-thread callback dispatcher for networking.
     * @ingroup EngineNetworking
     *
     * @c NetworkManager owns a pool of background worker threads. All
     * @c HttpClient and @c WebSocketClient requests run on those threads
     * so the render loop is never blocked. Completion callbacks are queued
     * and delivered on the main thread when @c flush() is called.
     *
     * ### Setup — call once in @c onInit()
     * @code
     * void MyApp::onInit() override
     * {
     *     ml::NetworkManager::initialize();   // start thread pool (default 4 threads)
     *     ml::NetworkManager::autoFlush();    // wire flush() into the frame loop
     * }
     * @endcode
     *
     * After that, every @c HttpClient and @c WebSocketClient callback fires
     * on the main thread automatically — no manual synchronisation required.
     *
     * ### Manual flush (advanced)
     * If you do not call @c autoFlush(), drain callbacks yourself each frame:
     * @code
     * void MyApp::onUpdate() { ml::NetworkManager::flush(); }
     * @endcode
     *
     * ### Shutdown
     * @c shutdown() is called automatically when @c AppManager destroys.
     * You only need to call it manually if you want to tear down the pool
     * before the application exits.
     *
     * @see HttpClient, WebSocketClient
     */
    class MALENA_API NetworkManager
    {
    public:
        NetworkManager()                               = delete;
        NetworkManager(const NetworkManager&)          = delete;
        NetworkManager& operator=(const NetworkManager&) = delete;

        // ── Lifecycle ─────────────────────────────────────────────────────────

        /**
         * @brief Start the background thread pool.
         *
         * Must be called before any @c HttpClient or @c WebSocketClient
         * sends a request. Safe to call multiple times — subsequent calls
         * are ignored if already initialised.
         *
         * @param threadCount  Number of worker threads. Defaults to 4.
         */
        static void initialize(std::size_t threadCount = 4);

        /**
         * @brief Stop all worker threads and discard pending work.
         *
         * In-flight callbacks that have already been queued to the main
         * thread are still delivered on the next @c flush(). Called
         * automatically at application exit.
         */
        static void shutdown();

        // ── Frame integration ─────────────────────────────────────────────────

        /**
         * @brief Register @c flush() with @c AppManager's pre-render hook.
         *
         * After this call, pending network callbacks are delivered once per
         * frame without any additional setup. Call this once in @c onInit().
         */
        static void autoFlush();

        /**
         * @brief Deliver all pending callbacks on the calling (main) thread.
         *
         * Called automatically each frame if @c autoFlush() was used.
         * Otherwise call this from @c onUpdate() to process responses.
         */
        static void flush();

        // ── Internal ─────────────────────────────────────────────────────────

        /// @cond INTERNAL

        /**
         * @brief Submit work to the thread pool with a main-thread completion.
         *
         * @p work runs on a background worker thread. When it completes,
         * @p onComplete is queued to run on the main thread at the next
         * @c flush().
         *
         * Used internally by @c HttpClient and @c WebSocketClient.
         * User code should not call this directly.
         *
         * @param work        Callable executed on a background thread.
         * @param onComplete  Callable queued to the main thread on completion.
         */
        static void enqueue(std::function<void()> work,
                            std::function<void()> onComplete);

        /**
         * @brief Queue a callback for main-thread delivery at the next flush.
         *
         * Used by @c WebSocketClient to deliver incoming messages and
         * connection events to the main thread. User code should not call
         * this directly.
         *
         * @param callback  Callable to invoke on the main thread.
         */
        static void queueCallback(std::function<void()> callback);

        /// @endcond
    };

} // namespace ml

#endif // MALENA_NETWORKMANAGER_H
