// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Engine/Networking/HttpClient.h>
#include <Malena/Engine/Networking/NetworkManager.h>

// cpp-httplib — header-only, included only in this translation unit
#include <httplib.h>

#include <stdexcept>

namespace ml
{
    // ── URL parsing helper ────────────────────────────────────────────────────

    namespace
    {
        struct ParsedUrl
        {
            std::string scheme;
            std::string host;
            int         port = 80;
            std::string path;
            bool        valid = false;
        };

        ParsedUrl parseUrl(const std::string& url)
        {
            ParsedUrl result;
            const auto schemeEnd = url.find("://");
            if (schemeEnd == std::string::npos) return result;

            result.scheme = url.substr(0, schemeEnd);
            std::string rest = url.substr(schemeEnd + 3);

            const auto pathStart = rest.find('/');
            std::string hostPort = (pathStart != std::string::npos)
                                   ? rest.substr(0, pathStart)
                                   : rest;
            result.path = (pathStart != std::string::npos)
                          ? rest.substr(pathStart)
                          : "/";

            const auto portPos = hostPort.find(':');
            if (portPos != std::string::npos)
            {
                result.host = hostPort.substr(0, portPos);
                result.port = std::stoi(hostPort.substr(portPos + 1));
            }
            else
            {
                result.host = hostPort;
                result.port = (result.scheme == "https") ? 443 : 80;
            }

            result.valid = !result.host.empty();
            return result;
        }

        // Build an httplib::Headers map from the request headers
        httplib::Headers toHttplibHeaders(
            const std::unordered_map<std::string, std::string>& h)
        {
            httplib::Headers out;
            for (const auto& [k, v] : h)
                out.emplace(k, v);
            return out;
        }

        // Convert an httplib::Response to a NetworkResponse
        NetworkResponse toNetworkResponse(const httplib::Result& result)
        {
            NetworkResponse resp;
            if (!result)
            {
                resp.error = httplib::to_string(result.error());
                return resp;
            }
            resp.statusCode = result->status;
            resp.body       = result->body;
            for (const auto& [k, v] : result->headers)
                resp.headers[k] = v;
            return resp;
        }
    } // anonymous namespace

    // ── HttpClient ────────────────────────────────────────────────────────────

    HttpClient::HttpClient(const std::string& baseUrl)
        : _baseUrl(baseUrl)
    {
    }

    void HttpClient::setBaseUrl(const std::string& url)   { _baseUrl = url; }
    void HttpClient::setDefaultTimeout(int seconds)        { _defaultTimeout = seconds; }

    void HttpClient::setDefaultHeader(const std::string& key,
                                       const std::string& value)
    {
        _defaultHeaders[key] = value;
    }

    void HttpClient::setBearerToken(const std::string& token)
    {
        _defaultHeaders["authorization"] = "Bearer " + token;
    }

    // ── Async helpers ─────────────────────────────────────────────────────────

    std::string HttpClient::resolveUrl(const std::string& path) const
    {
        if (path.find("://") != std::string::npos) return path; // already absolute
        return _baseUrl + path;
    }

    // ── Async public API ──────────────────────────────────────────────────────

    void HttpClient::send(const NetworkRequest& request, ResponseCallback cb)
    {
        auto result  = std::make_shared<NetworkResponse>();
        auto reqCopy = request;

        // Merge default headers
        for (const auto& [k, v] : _defaultHeaders)
            if (reqCopy.getHeaders().find(k) == reqCopy.getHeaders().end())
                reqCopy.header(k, v);

        NetworkManager::enqueue(
            [result, reqCopy]()
            {
                *result = HttpClient::execute(reqCopy);
            },
            [result, cb]()
            {
                cb(std::move(*result));
            }
        );
    }

    void HttpClient::get(const std::string& path, ResponseCallback cb)
    {
        send(NetworkRequest(resolveUrl(path)).method("GET"), std::move(cb));
    }

    void HttpClient::post(const std::string& path, const std::string& body,
                           ResponseCallback cb, const std::string& contentType)
    {
        send(NetworkRequest(resolveUrl(path)).method("POST").body(body, contentType),
             std::move(cb));
    }

    void HttpClient::put(const std::string& path, const std::string& body,
                          ResponseCallback cb, const std::string& contentType)
    {
        send(NetworkRequest(resolveUrl(path)).method("PUT").body(body, contentType),
             std::move(cb));
    }

    void HttpClient::patch(const std::string& path, const std::string& body,
                            ResponseCallback cb, const std::string& contentType)
    {
        send(NetworkRequest(resolveUrl(path)).method("PATCH").body(body, contentType),
             std::move(cb));
    }

    void HttpClient::del(const std::string& path, ResponseCallback cb)
    {
        send(NetworkRequest(resolveUrl(path)).method("DELETE"), std::move(cb));
    }

    // ── Sync ──────────────────────────────────────────────────────────────────

    NetworkResponse HttpClient::sendSync(const NetworkRequest& request)
    {
        return execute(request);
    }

    // ── Core execute (runs on background thread) ──────────────────────────────

    NetworkResponse HttpClient::execute(const NetworkRequest& request)
    {
        const auto parsed = parseUrl(request.getUrl());
        if (!parsed.valid)
        {
            NetworkResponse err;
            err.error = "Invalid URL: " + request.getUrl();
            return err;
        }

        const auto headers = toHttplibHeaders(request.getHeaders());

        const auto& method = request.getMethod();
        const auto& path   = parsed.path;
        const auto& body   = request.getBody();
        const auto& ct     = request.getContentType();
        const int   tout   = request.getTimeout();

        // Generic lambda — works with both httplib::Client and httplib::SSLClient
        // without requiring them to share a common base pointer.
        auto runRequest = [&](auto& cli) -> NetworkResponse
        {
            cli.set_connection_timeout(tout);
            cli.set_read_timeout(tout);

            if (method == "GET")    return toNetworkResponse(cli.Get(path, headers));
            if (method == "DELETE") return toNetworkResponse(cli.Delete(path, headers));
            if (method == "POST")   return toNetworkResponse(cli.Post(path, headers, body, ct));
            if (method == "PUT")    return toNetworkResponse(cli.Put(path, headers, body, ct));
            if (method == "PATCH")  return toNetworkResponse(cli.Patch(path, headers, body, ct));

            NetworkResponse err;
            err.error = "Unsupported HTTP method: " + method;
            return err;
        };

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        if (parsed.scheme == "https")
        {
            httplib::SSLClient cli(parsed.host, parsed.port);
            return runRequest(cli);
        }
#endif
        httplib::Client cli(parsed.host, parsed.port);
        return runRequest(cli);
    }

} // namespace ml
