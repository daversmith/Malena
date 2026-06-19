// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_JSON_H
#define MALENA_JSON_H

#pragma once

#include <nlohmann/json.hpp>

namespace ml
{
    /**
     * @brief Alias for @c nlohmann::json.
     * @ingroup Utilities
     *
     * Brings the nlohmann JSON type into the @c ml namespace so you can
     * use @c ml::json throughout your application without qualifying the
     * full library namespace.
     *
     * @code
     * ml::json body = {{"prompt", "What is RAII?"}, {"type", "SHORT_ESSAY"}};
     * _http.post("/questions", body, [](ml::NetworkResponse resp) { ... });
     *
     * // Parsing a response
     * auto data = ml::json::parse(resp.body);
     * std::string prompt = data["prompt"];
     * @endcode
     *
     * @see HttpClient, WebSocketClient
     */
    using json = nlohmann::json;

} // namespace ml

#endif // MALENA_JSON_H
