// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_UTILITIES_PATHS_H
#define MALENA_UTILITIES_PATHS_H

#include <Malena/Core/malena_export.h>
#include <string>

namespace ml {

/**
 * @brief Where an installed app finds its assets and keeps its user data.
 *
 * A development build runs from its build directory, so relative paths like
 * "assets/logo.png" resolve against the current working directory and everything
 * works. An *installed* app has neither property:
 *
 *  - Launched from Finder, a macOS .app has a working directory of "/" — every
 *    CWD-relative asset path misses.
 *  - Writing next to the executable fails once the app lives in /Applications or
 *    C:\Program Files, which are not user-writable.
 *
 * These two helpers give the app an install-safe answer on every platform. Use
 * resourceDir() for read-only files that ship with the app (assets, bundled
 * content) and userDataDir() for anything the app writes (config, save data,
 * flags). Both return paths WITHOUT a trailing separator.
 */
class MALENA_API Paths
{
public:
    /**
     * Absolute path to the directory holding the running executable, except on
     * macOS inside a .app bundle, where it's the bundle's Contents/Resources —
     * the conventional location for bundled assets.
     *
     * Returns "." if the executable path can't be determined, which preserves
     * the CWD-relative behaviour a dev build already relies on.
     */
    static const std::string& resourceDir();

    /**
     * Absolute path to a per-user, writable directory for this app, created on
     * first call:
     *   macOS:   ~/Library/Application Support/<appName>
     *   Windows: %APPDATA%\<appName>
     *   Linux:   $XDG_CONFIG_HOME/<appName>, else ~/.config/<appName>
     *
     * Falls back to "." if neither the home directory nor the platform variable
     * is available, and if the directory can't be created.
     *
     * @param appName Folder name to use — the application's name.
     */
    static std::string userDataDir(const std::string& appName);

    /** Join two path segments with the platform separator. */
    static std::string join(const std::string& a, const std::string& b);

    /** Absolute path to the running executable ("" if undeterminable). */
    static const std::string& executablePath();
};

} // namespace ml

#endif // MALENA_UTILITIES_PATHS_H
