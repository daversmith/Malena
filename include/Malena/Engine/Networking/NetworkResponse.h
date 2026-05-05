// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_NETWORKRESPONSE_H
#define MALENA_NETWORKRESPONSE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <string>
#include <unordered_map>

namespace ml
{
    /**
     * @brief The result of an HTTP or WebSocket message.
     * @ingroup EngineNetworking
     *
     * Returned to every @c HttpClient callback and carried by
     * @c WebSocketClient message events. Check @c ok() before accessing
     * @c body — if the transport failed, @c error describes the reason and
     * @c statusCode is 0.
     *
     * @code
     * client.get("/api/questions", [](ml::NetworkResponse resp) {
     *     if (!resp.ok()) {
     *         std::cerr << "Request failed: " << resp.error << "\n";
     *         return;
     *     }
     *     // resp.statusCode == 200, resp.body contains JSON
     * });
     * @endcode
     *
     * @see HttpClient, WebSocketClient
     */
    struct MALENA_API NetworkResponse
    {
        /// HTTP status code (200, 404, 500 …). 0 on transport/connection failure.
        int statusCode = 0;

        /// Response body as a UTF-8 string.
        std::string body;

        /// Response headers, lower-cased keys.
        std::unordered_map<std::string, std::string> headers;

        /// Non-empty when a transport or connection error occurred.
        std::string error;

        /**
         * @brief Return @c true when the request succeeded with a 2xx status.
         *
         * A response is considered successful when there is no transport error
         * and the HTTP status is in the [200, 299] range.
         */
        [[nodiscard]] bool ok() const
        {
            return error.empty() && statusCode >= 200 && statusCode < 300;
        }

        /**
         * @brief Return @c true when a header with @p key is present.
         * @param key  Header name (case-insensitive — stored lower-cased).
         */
        [[nodiscard]] bool hasHeader(const std::string& key) const
        {
            return headers.count(key) > 0;
        }

        /**
         * @brief Return the value of header @p key, or @p fallback if absent.
         * @param key      Header name (case-insensitive).
         * @param fallback Value returned when the header is not present.
         */
        [[nodiscard]] const std::string& header(const std::string& key,
                                                 const std::string& fallback = "") const
        {
            auto it = headers.find(key);
            return it != headers.end() ? it->second : fallback;
        }
    };

} // namespace ml

#endif // MALENA_NETWORKRESPONSE_H
