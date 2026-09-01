// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_SYSTEMOPEN_H
#define MALENA_SYSTEMOPEN_H

#include <string>

namespace ml {

/**
 * Hand a URI to the operating system, the way clicking a link does.
 *
 * Takes http(s):// as well as the settings schemes each platform exposes
 * (`ms-settings:` on Windows, `x-apple.systempreferences:` on macOS), so an app
 * can send someone straight to the panel they need instead of describing where
 * to find it.
 *
 * Returns whether the launch was successfully HANDED OFF, not whether the target
 * opened: on POSIX that means exec succeeded, and the helper it runs (`open`,
 * `xdg-open`) may still reject the URI afterwards. Learning the real outcome
 * means waiting on that helper, which blocks the calling thread for as long as a
 * cold application takes to start.
 *
 * Callers should treat false as "tell them where to go instead", never as a
 * fatal error - an app that cannot open a settings panel is still a working app.
 */
bool systemOpen(const std::string& uri);

/**
 * Open the Wi-Fi settings panel.
 *
 * Separate from systemOpen because the URI differs per platform and every caller
 * would otherwise repeat the same #ifdef. Wi-Fi is the case worth wrapping: an
 * app that needs a particular network can put the user one click away from
 * choosing it, which is as far as it can go without the permission to read or
 * change networks itself.
 */
bool openWifiSettings();

} // namespace ml

#endif // MALENA_SYSTEMOPEN_H
