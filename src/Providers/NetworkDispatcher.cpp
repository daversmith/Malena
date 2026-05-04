// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Providers/NetworkDispatcher.h>

namespace ml
{
    NetworkDispatcher::NetworkDispatcher(const std::string& keyField)
        : _keyField(keyField) {}

    void NetworkDispatcher::on(const std::string& value, std::function<void(const json&)> callback)
    {
        onMessage<json>(value, std::move(callback));
    }

    void NetworkDispatcher::dispatch(const json& msg)
    {
        const std::string key = msg.value(_keyField, "");
        if (!key.empty())
            sendMessage<json>(key, msg);
    }

} // namespace ml
