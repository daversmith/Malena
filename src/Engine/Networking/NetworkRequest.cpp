// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Engine/Networking/NetworkRequest.h>
#include <stdexcept>

// ── Base64 helper (for Basic Auth) ────────────────────────────────────────────
namespace
{
    static const char kB64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string base64Encode(const std::string& in)
    {
        std::string out;
        int val = 0, valb = -6;
        for (unsigned char c : in)
        {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0)
            {
                out.push_back(kB64Chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6)
            out.push_back(kB64Chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4)
            out.push_back('=');
        return out;
    }
} // anonymous namespace

namespace ml
{
    NetworkRequest::NetworkRequest(const std::string& url)
        : _url(url)
    {
    }

    NetworkRequest& NetworkRequest::method(const std::string& m)
    {
        _method = m;
        return *this;
    }

    NetworkRequest& NetworkRequest::header(const std::string& key,
                                            const std::string& value)
    {
        _headers[key] = value;
        return *this;
    }

    NetworkRequest& NetworkRequest::bearerToken(const std::string& token)
    {
        _headers["authorization"] = "Bearer " + token;
        return *this;
    }

    NetworkRequest& NetworkRequest::basicAuth(const std::string& user,
                                               const std::string& pass)
    {
        _headers["authorization"] = "Basic " + base64Encode(user + ":" + pass);
        return *this;
    }

    NetworkRequest& NetworkRequest::body(const std::string& data,
                                          const std::string& contentType)
    {
        _body        = data;
        _contentType = contentType;
        return *this;
    }

    NetworkRequest& NetworkRequest::timeout(int seconds)
    {
        _timeoutSeconds = seconds;
        return *this;
    }

} // namespace ml
