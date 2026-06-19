// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Providers/WebSocketProvider.h>
#include <Malena/Engine/Networking/NetworkManager.h>

namespace ml
{
    WebSocketProvider::WebSocketProvider(const std::string& keyField)
        : NetworkDispatcher(keyField)
    {
        NetworkManager::initialize();

        _ws.onMessage([this](const std::string& raw) {
            try {
                dispatch(json::parse(raw));
            } catch (...) {}
        });
    }

    void WebSocketProvider::connect(const std::string& url)    { _ws.connect(url); }
    void WebSocketProvider::disconnect()                        { _ws.disconnect(); }
    void WebSocketProvider::send(const json& msg)               { _ws.send(msg); }
    void WebSocketProvider::send(const std::string& text)       { _ws.send(text); }

    void WebSocketProvider::onOpen(std::function<void()> cb)
    {
        _ws.onOpen(std::move(cb));
    }

    void WebSocketProvider::onClose(std::function<void(int, const std::string&)> cb)
    {
        _ws.onClose(std::move(cb));
    }

    void WebSocketProvider::onError(std::function<void(const std::string&)> cb)
    {
        _ws.onError(std::move(cb));
    }

    void WebSocketProvider::setReconnectDelay(int seconds)      { _ws.setReconnectDelay(seconds); }
    void WebSocketProvider::setMaxReconnectAttempts(int n)      { _ws.setMaxReconnectAttempts(n); }

} // namespace ml
