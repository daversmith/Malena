// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_WEBSOCKETCLIENT_H
#define MALENA_WEBSOCKETCLIENT_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace ml
{
    /**
     * @brief Persistent WebSocket client with main-thread callback delivery.
     * @ingroup EngineNetworking
     *
     * @c WebSocketClient maintains a single WebSocket connection on a
     * background thread. All callbacks — @c onOpen, @c onMessage,
     * @c onClose, @c onError — are queued and delivered on the main thread
     * at the next frame flush, so you can safely update UI state inside them.
     *
     * ### Basic usage
     * @code
     * ml::WebSocketClient ws;
     *
     * ws.onOpen([this]() {
     *     ws.send(R"({"type":"subscribe","channel":"questions"})");
     * });
     *
     * ws.onMessage([this](const std::string& msg) {
     *     handleServerEvent(msg);
     * });
     *
     * ws.onClose([](int code, const std::string& reason) {
     *     std::cout << "Closed " << code << ": " << reason << "\n";
     * });
     *
     * ws.onError([](const std::string& err) {
     *     std::cerr << "WS error: " << err << "\n";
     * });
     *
     * ws.connect("wss://api.example.com/events");
     * @endcode
     *
     * ### Reconnection
     * By default the client does not reconnect on unexpected closure.
     * Enable automatic reconnection with a back-off delay:
     * @code
     * ws.setReconnectDelay(2);          // try again after 2 s
     * ws.setMaxReconnectAttempts(5);    // give up after 5 tries (-1 = unlimited)
     * @endcode
     *
     * @see HttpClient, NetworkManager
     */
    class MALENA_API WebSocketClient
    {
    public:
        using MessageCallback = std::function<void(const std::string& message)>;
        using BinaryCallback  = std::function<void(const std::vector<uint8_t>& data)>;
        using OpenCallback    = std::function<void()>;
        using CloseCallback   = std::function<void(int code, const std::string& reason)>;
        using ErrorCallback   = std::function<void(const std::string& error)>;

        WebSocketClient();
        ~WebSocketClient();

        WebSocketClient(const WebSocketClient&)            = delete;
        WebSocketClient& operator=(const WebSocketClient&) = delete;

        // ── Connection ───────────────────────────────────────────────────────

        /**
         * @brief Connect to @p url on a background thread.
         *
         * Supports @c ws:// and @c wss:// (TLS) schemes.
         * Fires @c onOpen when the handshake completes.
         *
         * @param url  WebSocket URL, e.g. @c "wss://api.example.com/events".
         */
        void connect(const std::string& url);

        /**
         * @brief Close the connection gracefully.
         *
         * Fires @c onClose with code 1000 (normal closure). Safe to call
         * when already disconnected.
         */
        void disconnect();

        /** @brief Return @c true while the connection is open. */
        [[nodiscard]] bool isConnected() const;

        // ── Sending ──────────────────────────────────────────────────────────

        /**
         * @brief Send a UTF-8 text frame.
         * @param message  Text payload.
         */
        void send(const std::string& message);

        /**
         * @brief Send a binary frame.
         * @param data  Binary payload.
         */
        void sendBinary(const std::vector<uint8_t>& data);

        /**
         * @brief Serialise @p json and send it as a UTF-8 text frame.
         *
         * Equivalent to calling @c send(json.dump()).
         *
         * @code
         * ws.send({{"type", "subscribe"}, {"channel", "questions"}});
         * ws.send({{"type", "answer"}, {"questionId", 7}, {"value", "RAII"}});
         * @endcode
         */
        void send(const nlohmann::json& json)
        {
            send(json.dump());
        }

        // ── Callbacks ────────────────────────────────────────────────────────

        /**
         * @brief Fired on the main thread when the connection opens.
         *
         * Use this to send an initial subscription message.
         */
        void onOpen(OpenCallback cb);

        /**
         * @brief Fired on the main thread when a UTF-8 text frame arrives.
         * @param cb  Callback receiving the text payload.
         */
        void onMessage(MessageCallback cb);

        /**
         * @brief Fired on the main thread when a binary frame arrives.
         * @param cb  Callback receiving the binary payload.
         */
        void onBinary(BinaryCallback cb);

        /**
         * @brief Fired on the main thread when the connection closes.
         * @param cb  Callback receiving the WebSocket close code and reason.
         */
        void onClose(CloseCallback cb);

        /**
         * @brief Fired on the main thread when a protocol or network error occurs.
         * @param cb  Callback receiving an error description.
         */
        void onError(ErrorCallback cb);

        // ── Reconnection ─────────────────────────────────────────────────────

        /**
         * @brief Set automatic reconnect delay in seconds.
         *
         * When non-zero, the client reconnects after an unexpected closure.
         * The delay doubles on each failed attempt up to @c maxReconnectAttempts.
         *
         * @param seconds  Delay before the first reconnect attempt. 0 = disabled.
         */
        void setReconnectDelay(int seconds);

        /**
         * @brief Maximum number of reconnection attempts before giving up.
         * @param n  Pass @c -1 for unlimited attempts.
         */
        void setMaxReconnectAttempts(int n);

    private:
        /// @cond INTERNAL
        struct Impl;
        std::unique_ptr<Impl> _impl;
        /// @endcond
    };

} // namespace ml

#endif // MALENA_WEBSOCKETCLIENT_H
