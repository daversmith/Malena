// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_NETWORKREQUEST_H
#define MALENA_NETWORKREQUEST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace ml
{
    /**
     * @brief Fluent builder for an outgoing HTTP request.
     * @ingroup EngineNetworking
     *
     * Construct a @c NetworkRequest with a URL, then chain method calls to
     * configure headers, body, and other options before passing it to
     * @c HttpClient::send().
     *
     * @code
     * auto req = ml::NetworkRequest("https://api.example.com/submit")
     *     .method("POST")
     *     .body(R"({"answer":"42"})")
     *     .bearerToken(myToken)
     *     .timeout(10);
     *
     * client.send(req, [](ml::NetworkResponse resp){ ... });
     * @endcode
     *
     * @see HttpClient, NetworkResponse
     */
    class MALENA_API NetworkRequest
    {
    public:
        /**
         * @brief Construct a request for @p url.
         *
         * The method defaults to @c GET. Add headers and a body before
         * sending.
         *
         * @param url  Fully-qualified URL including scheme
         *             (@c http:// or @c https://).
         */
        explicit NetworkRequest(const std::string& url);

        // ── Method ───────────────────────────────────────────────────────────

        /**
         * @brief Override the HTTP method.
         *
         * Defaults to @c "GET". Common values: @c "POST", @c "PUT",
         * @c "PATCH", @c "DELETE".
         */
        NetworkRequest& method(const std::string& m);

        // ── Headers ──────────────────────────────────────────────────────────

        /**
         * @brief Add or replace a request header.
         * @param key    Header name (e.g. @c "X-Request-ID").
         * @param value  Header value.
         */
        NetworkRequest& header(const std::string& key, const std::string& value);

        /**
         * @brief Set the @c Authorization header to @c "Bearer <token>".
         * @param token  Bearer token string.
         */
        NetworkRequest& bearerToken(const std::string& token);

        /**
         * @brief Set HTTP Basic Authentication credentials.
         *
         * Encodes @p user and @p pass as Base64 and sets the
         * @c Authorization header.
         */
        NetworkRequest& basicAuth(const std::string& user, const std::string& pass);

        // ── Body ─────────────────────────────────────────────────────────────

        /**
         * @brief Set the request body.
         *
         * @param data         Raw body string.
         * @param contentType  Value for the @c Content-Type header.
         *                     Defaults to @c "application/json".
         */
        NetworkRequest& body(const std::string& data,
                             const std::string& contentType = "application/json");

        /**
         * @brief Set the request body from a @c nlohmann::json value.
         *
         * Serialises @p json to a string and sets @c Content-Type to
         * @c application/json automatically.
         *
         * @code
         * auto req = ml::NetworkRequest("https://api.example.com/submit")
         *     .method("POST")
         *     .body({{"answer", "42"}, {"questionId", 7}})
         *     .bearerToken(token);
         * @endcode
         */
        NetworkRequest& body(const nlohmann::json& json)
        {
            return body(json.dump(), "application/json");
        }

        // ── Options ──────────────────────────────────────────────────────────

        /**
         * @brief Set the request timeout in seconds. Default is 30 s.
         * @param seconds  Maximum time to wait for a complete response.
         */
        NetworkRequest& timeout(int seconds);

        // ── Accessors ────────────────────────────────────────────────────────

        [[nodiscard]] const std::string& getUrl()         const { return _url; }
        [[nodiscard]] const std::string& getMethod()      const { return _method; }
        [[nodiscard]] const std::string& getBody()        const { return _body; }
        [[nodiscard]] const std::string& getContentType() const { return _contentType; }
        [[nodiscard]] int                getTimeout()     const { return _timeoutSeconds; }

        [[nodiscard]] const std::unordered_map<std::string, std::string>&
        getHeaders() const { return _headers; }

    private:
        std::string _url;
        std::string _method        = "GET";
        std::string _body;
        std::string _contentType;
        int         _timeoutSeconds = 30;

        std::unordered_map<std::string, std::string> _headers;
    };

} // namespace ml

#endif // MALENA_NETWORKREQUEST_H
