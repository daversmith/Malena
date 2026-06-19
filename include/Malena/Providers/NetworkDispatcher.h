// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_NETWORK_DISPATCHER_H
#define MALENA_NETWORK_DISPATCHER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Communication/Messenger.h>
#include <Malena/Utilities/Json.h>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief Routes incoming JSON messages to typed message-bus subscribers.
     * @ingroup Providers
     *
     * @c NetworkDispatcher reads a configurable key field (default @c "type")
     * from each incoming JSON object and publishes its value on the global
     * message bus. Subscribers call @c on() with the value they care about:
     *
     * @code
     * // Default key field ("type")
     * NetworkDispatcher dispatcher;
     * dispatcher.on("student_connected", [](const ml::json& msg) { ... });
     *
     * // Custom key field — server uses "event" instead of "type"
     * NetworkDispatcher dispatcher{"event"};
     * dispatcher.on("courses", [](const ml::json& msg) { ... });
     * @endcode
     *
     * @see WebSocketProvider
     */
    class MALENA_API NetworkDispatcher : public Messenger
    {
    public:
        explicit NetworkDispatcher(const std::string& keyField = "type");

        /**
         * @brief Subscribe a callback for a specific message value.
         *
         * @param value     The value of the key field to listen for.
         * @param callback  Called with the full JSON object when the value matches.
         */
        void on(const std::string& value, std::function<void(const json&)> callback);

        /**
         * @brief Dispatch a JSON object to the matching subscriber.
         *
         * Reads @c _keyField from the message and publishes its value on the
         * message bus. No-op if the field is missing or empty.
         *
         * @param msg  Parsed JSON message from the network.
         */
        void dispatch(const json& msg);

    private:
        std::string _keyField;
    };

} // namespace ml

#endif // MALENA_NETWORK_DISPATCHER_H
