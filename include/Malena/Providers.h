// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_PROVIDERS_H
#define MALENA_PROVIDERS_H

#pragma once

/**
 * @defgroup Providers Providers
 * @brief Reusable resource managers that abstract external services.
 *
 * Providers combine raw transport classes with higher-level dispatch so that
 * application code only interacts through a clean @c on() / @c send() API:
 *
 * - @ref ml::NetworkDispatcher — routes JSON by @c "type" field to subscribers
 * - @ref ml::WebSocketProvider — WebSocket connection + JSON dispatch in one object
 */

#include <Malena/Providers/NetworkDispatcher.h>
#include <Malena/Providers/WebSocketProvider.h>

#endif // MALENA_PROVIDERS_H
