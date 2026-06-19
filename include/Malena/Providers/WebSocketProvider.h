// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_WEBSOCKET_PROVIDER_H
#define MALENA_WEBSOCKET_PROVIDER_H

#pragma once

#include <Malena/Providers/NetworkDispatcher.h>
#include <Malena/Engine/Networking/WebSocketClient.h>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief WebSocket connection with automatic JSON dispatch.
     * @ingroup Providers
     *
     * @c WebSocketProvider combines a @c WebSocketClient (raw transport) with a
     * @c NetworkDispatcher (typed routing) into a single reusable object.
     *
     * Create one in your application class, call @c on() in each hook's @c init()
     * to declare which message types it cares about, then call @c connect() once
     * to open the connection:
     *
     * @code
     * // In your application constructor:
     * _network.onOpen([this]{
     *     _network.send({{"type","register"}, {"role","admin"}});
     * });
     * _network.connect("ws://localhost:3000");
     *
     * _chatHook->init(_network);
     * _studentsHook->init(_network);
     * @endcode
     *
     * @code
     * // In a hook's init():
     * void ChatHook::init(ml::WebSocketProvider& network)
     * {
     *     network.on("student_message", [this](const ml::json& msg) {
     *         handleStudentMessage(msg["from"], msg["text"], msg.value("ts", ""));
     *     });
     *     sendHook(AppShell::Hook::GRAPHIC);
     * }
     * @endcode
     *
     * @see NetworkDispatcher, WebSocketClient
     */
    class MALENA_API WebSocketProvider : public NetworkDispatcher
    {
    public:
        explicit WebSocketProvider(const std::string& keyField = "type");

        // ── Connection ───────────────────────────────────────────────────────

        /**
         * @brief Open a WebSocket connection to @p url.
         *
         * Automatically parses every incoming text frame as JSON and dispatches
         * it to subscribers registered with @c on(). Non-JSON frames are silently
         * ignored.
         *
         * @param url  @c ws:// or @c wss:// URL.
         */
        void connect(const std::string& url);

        /** @brief Close the connection gracefully. */
        void disconnect();

        /** @brief Send a JSON object as a UTF-8 text frame. */
        void send(const json& msg);

        /** @brief Send a raw text frame. */
        void send(const std::string& text);

        // ── Lifecycle callbacks ──────────────────────────────────────────────

        /** @brief Called on the main thread when the connection opens. */
        void onOpen(std::function<void()> callback);

        /** @brief Called on the main thread when the connection closes. */
        void onClose(std::function<void(int, const std::string&)> callback);

        /** @brief Called on the main thread on a protocol or network error. */
        void onError(std::function<void(const std::string&)> callback);

        // ── Reconnection ─────────────────────────────────────────────────────

        /** @brief Delay in seconds before reconnecting after an unexpected close. */
        void setReconnectDelay(int seconds);

        /** @brief Maximum reconnect attempts. Pass @c -1 for unlimited. */
        void setMaxReconnectAttempts(int n);

    private:
        WebSocketClient _ws;
    };

} // namespace ml

#endif // MALENA_WEBSOCKET_PROVIDER_H
