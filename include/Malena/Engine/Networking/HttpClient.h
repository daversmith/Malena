// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_HTTPCLIENT_H
#define MALENA_HTTPCLIENT_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Networking/NetworkRequest.h>
#include <Malena/Engine/Networking/NetworkResponse.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace ml
{
    /**
     * @brief Async and sync HTTP client backed by the background thread pool.
     * @ingroup EngineNetworking
     *
     * All async methods submit the request to @c NetworkManager's thread pool
     * and deliver the @c NetworkResponse callback on the main thread at the
     * next frame flush — keeping the render loop unblocked.
     *
     * ### Async (recommended)
     * @code
     * ml::HttpClient client("https://api.example.com");
     * client.setBearerToken(myToken);
     *
     * // GET /questions — callback runs on the main thread
     * client.get("/questions", [this](ml::NetworkResponse resp) {
     *     if (!resp.ok()) return;
     *     loadQuestions(resp.body);
     * });
     *
     * // POST with JSON body
     * client.post("/answers", R"({"answer":"42"})",
     *     [](ml::NetworkResponse resp) { ... });
     * @endcode
     *
     * ### Sync (use sparingly — blocks the calling thread)
     * @code
     * auto resp = client.sendSync(
     *     ml::NetworkRequest("https://api.example.com/ping").timeout(5));
     * @endcode
     *
     * ### Per-request customisation
     * Build a @c NetworkRequest manually for full control:
     * @code
     * auto req = ml::NetworkRequest("https://api.example.com/upload")
     *     .method("PUT")
     *     .header("X-Upload-ID", uploadId)
     *     .body(data, "application/octet-stream")
     *     .timeout(60);
     *
     * client.send(req, [](ml::NetworkResponse resp) { ... });
     * @endcode
     *
     * @see NetworkRequest, NetworkResponse, NetworkManager
     */
    class MALENA_API HttpClient
    {
    public:
        using ResponseCallback = std::function<void(NetworkResponse)>;

        /**
         * @brief Construct an @c HttpClient with an optional base URL.
         *
         * @param baseUrl  Scheme + host (+ optional port) prepended to every
         *                 relative path. E.g. @c "https://api.example.com".
         *                 Leave empty when passing fully-qualified URLs directly.
         */
        explicit HttpClient(const std::string& baseUrl = "");

        // ── Configuration ────────────────────────────────────────────────────

        /** @brief Set or replace the base URL (scheme + host + optional port). */
        void setBaseUrl(const std::string& url);

        /**
         * @brief Add a default header sent with every request.
         *
         * Overridden per-request if the same key is set on a @c NetworkRequest.
         */
        void setDefaultHeader(const std::string& key, const std::string& value);

        /** @brief Set the default timeout in seconds (default 30). */
        void setDefaultTimeout(int seconds);

        /**
         * @brief Set the @c Authorization: Bearer token applied to all requests.
         *
         * Equivalent to calling @c setDefaultHeader("authorization", "Bearer <token>").
         */
        void setBearerToken(const std::string& token);

        // ── Async ────────────────────────────────────────────────────────────

        /**
         * @brief Send a @c GET request asynchronously.
         * @param path  URL path (appended to @c baseUrl) or a fully-qualified URL.
         * @param cb    Callback invoked on the main thread with the response.
         */
        void get(const std::string& path, ResponseCallback cb);

        /**
         * @brief Send a @c POST request with a body asynchronously.
         * @param path         URL path or fully-qualified URL.
         * @param body         Request body string.
         * @param cb           Callback invoked on the main thread.
         * @param contentType  @c Content-Type header value. Default: @c "application/json".
         */
        void post(const std::string& path,
                  const std::string& body,
                  ResponseCallback   cb,
                  const std::string& contentType = "application/json");

        /**
         * @brief Send a @c PUT request with a body asynchronously.
         */
        void put(const std::string& path,
                 const std::string& body,
                 ResponseCallback   cb,
                 const std::string& contentType = "application/json");

        /**
         * @brief Send a @c PATCH request with a body asynchronously.
         */
        void patch(const std::string& path,
                   const std::string& body,
                   ResponseCallback   cb,
                   const std::string& contentType = "application/json");

        /**
         * @brief Send a @c DELETE request asynchronously.
         */
        void del(const std::string& path, ResponseCallback cb);

        // ── JSON overloads ───────────────────────────────────────────────────

        /**
         * @brief Send a @c POST request with a @c nlohmann::json body.
         *
         * Serialises @p json to a string and sets @c Content-Type to
         * @c application/json automatically.
         */
        void post(const std::string&       path,
                  const nlohmann::json&    json,
                  ResponseCallback         cb)
        {
            post(path, json.dump(), std::move(cb));
        }

        /**
         * @brief Send a @c PUT request with a @c nlohmann::json body.
         */
        void put(const std::string&       path,
                 const nlohmann::json&    json,
                 ResponseCallback         cb)
        {
            put(path, json.dump(), std::move(cb));
        }

        /**
         * @brief Send a @c PATCH request with a @c nlohmann::json body.
         */
        void patch(const std::string&       path,
                   const nlohmann::json&    json,
                   ResponseCallback         cb)
        {
            patch(path, json.dump(), std::move(cb));
        }

        /**
         * @brief Send an arbitrary @c NetworkRequest asynchronously.
         *
         * Default headers and the bearer token are merged in unless the
         * request already sets those keys.
         */
        void send(const NetworkRequest& request, ResponseCallback cb);

        // ── Sync ─────────────────────────────────────────────────────────────

        /**
         * @brief Execute @p request synchronously on the calling thread.
         *
         * @warning Blocks until the response arrives or the timeout fires.
         *          Never call this from the main render loop — use the async
         *          overloads instead.
         *
         * @param request  The request to execute.
         * @return         The server's response.
         */
        NetworkResponse sendSync(const NetworkRequest& request);

    private:
        std::string _baseUrl;
        int         _defaultTimeout = 30;
        std::unordered_map<std::string, std::string> _defaultHeaders;

        /// Execute @p request on the calling thread. Used by both sync and async paths.
        static NetworkResponse execute(const NetworkRequest& request);

        /// Build a fully-qualified URL from @p path and @c _baseUrl.
        [[nodiscard]] std::string resolveUrl(const std::string& path) const;
    };

} // namespace ml

#endif // MALENA_HTTPCLIENT_H
