// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Engine/Networking/WebSocketClient.h>
#include <Malena/Engine/Networking/NetworkManager.h>

// IXWebSocket — included only in this translation unit
#include <ixwebsocket/IXWebSocket.h>

#include <atomic>
#include <mutex>

namespace ml
{
    // ── Impl ──────────────────────────────────────────────────────────────────

    struct WebSocketClient::Impl
    {
        ix::WebSocket ws;

        // User callbacks
        WebSocketClient::OpenCallback    cbOpen;
        WebSocketClient::MessageCallback cbMessage;
        WebSocketClient::BinaryCallback  cbBinary;
        WebSocketClient::CloseCallback   cbClose;
        WebSocketClient::ErrorCallback   cbError;

        std::atomic<bool> connected{ false };
        int  reconnectDelaySec     = 0;
        int  maxReconnectAttempts  = 0;

        Impl()
        {
            ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg)
            {
                switch (msg->type)
                {
                    case ix::WebSocketMessageType::Open:
                        connected = true;
                        if (cbOpen)
                            NetworkManager::queueCallback([cb = cbOpen]{ cb(); });
                        break;

                    case ix::WebSocketMessageType::Message:
                        if (msg->binary)
                        {
                            if (cbBinary)
                            {
                                std::vector<uint8_t> data(msg->str.begin(),
                                                          msg->str.end());
                                NetworkManager::queueCallback(
                                    [cb = cbBinary, d = std::move(data)]{ cb(d); });
                            }
                        }
                        else
                        {
                            if (cbMessage)
                            {
                                NetworkManager::queueCallback(
                                    [cb = cbMessage, s = msg->str]{ cb(s); });
                            }
                        }
                        break;

                    case ix::WebSocketMessageType::Close:
                        connected = false;
                        if (cbClose)
                        {
                            int         code   = msg->closeInfo.code;
                            std::string reason = msg->closeInfo.reason;
                            NetworkManager::queueCallback(
                                [cb = cbClose, code, reason]{ cb(code, reason); });
                        }
                        break;

                    case ix::WebSocketMessageType::Error:
                        if (cbError)
                        {
                            NetworkManager::queueCallback(
                                [cb = cbError, s = msg->errorInfo.reason]{ cb(s); });
                        }
                        break;

                    default:
                        break;
                }
            });
        }
    };

    // ── WebSocketClient ───────────────────────────────────────────────────────

    WebSocketClient::WebSocketClient()
        : _impl(std::make_unique<Impl>())
    {
    }

    WebSocketClient::~WebSocketClient()
    {
        disconnect();
    }

    void WebSocketClient::connect(const std::string& url)
    {
        _impl->ws.setUrl(url);

        if (_impl->reconnectDelaySec > 0)
        {
            ix::WebSocketPerMessageDeflateOptions options;
            _impl->ws.enableAutomaticReconnection();
            _impl->ws.setMaxWaitBetweenReconnectionRetries(
                static_cast<uint32_t>(_impl->reconnectDelaySec) * 1000);
        }
        else
        {
            _impl->ws.disableAutomaticReconnection();
        }

        _impl->ws.start();
    }

    void WebSocketClient::disconnect()
    {
        _impl->ws.stop();
        _impl->connected = false;
    }

    bool WebSocketClient::isConnected() const
    {
        return _impl->connected;
    }

    void WebSocketClient::send(const std::string& message)
    {
        _impl->ws.send(message);
    }

    void WebSocketClient::sendBinary(const std::vector<uint8_t>& data)
    {
        _impl->ws.sendBinary(std::string(data.begin(), data.end()));
    }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void WebSocketClient::onOpen   (OpenCallback    cb) { _impl->cbOpen    = std::move(cb); }
    void WebSocketClient::onMessage(MessageCallback cb) { _impl->cbMessage = std::move(cb); }
    void WebSocketClient::onBinary (BinaryCallback  cb) { _impl->cbBinary  = std::move(cb); }
    void WebSocketClient::onClose  (CloseCallback   cb) { _impl->cbClose   = std::move(cb); }
    void WebSocketClient::onError  (ErrorCallback   cb) { _impl->cbError   = std::move(cb); }

    // ── Reconnection ──────────────────────────────────────────────────────────

    void WebSocketClient::setReconnectDelay(int seconds)
    {
        _impl->reconnectDelaySec = seconds;
    }

    void WebSocketClient::setMaxReconnectAttempts(int n)
    {
        _impl->maxReconnectAttempts = n;
    }

} // namespace ml
