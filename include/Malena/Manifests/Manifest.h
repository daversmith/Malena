// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MANIFEST_H
#define MANIFEST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <unordered_map>
#include <memory>
#include <string>
#include <type_traits>
#include <Malena/Utilities/EnumClassHash.h>
#include <Malena/Manifests/ThemeTag.h>

namespace ml
{
    /**
     * @brief Base class for all Malena manifests.
     * @ingroup Manifests
     *
     * A manifest is a static registry that maps enum keys to file paths or
     * configuration values. Subclasses declare inner enum types and populate
     * them via the protected @c set() methods in an inline static initializer.
     *
     * Theme-derived values are stored polymorphically as @c shared_ptr<ThemeTag>
     * and retrieved by @c ThemeManager::apply() — no manual casting required.
     *
     * ### Usage
     * @code
     * class MyManifest : public ml::Manifest
     * {
     * public:
     *     enum class Themes  { Dark, Neon };
     *     enum class Images  { Background };
     *     enum class Ints    { MaxPlayers };
     *
     *     inline static const auto _ = [](){
     *         set(Themes::Dark,       ml::DarkTheme());
     *         set(Images::Background, "assets/bg.png");
     *         set(Ints::MaxPlayers,   4);
     *         return 0;
     *     }();
     * };
     * @endcode
     *
     * @see ThemeManager, AssetsManager, ConfigManager
     */
    class MALENA_API Manifest
    {
    public:
        // ── Retrieval ─────────────────────────────────────────────────────────

        template<typename Asset>
        static const std::string& getFilepath(const Asset& asset);

        template<typename ConfigType, typename ValueType>
        static const ValueType& getConfig(ConfigType config);

        /**
         * @brief Retrieve a theme stored via @c set(key, ThemeDerived{}).
         *
         * Called internally by @c ThemeManager::apply(). Returns a reference
         * to the stored theme cast to @c ThemeTag — ThemeManager casts to
         * @c Theme& from there.
         */
        template<typename EnumType>
        static const ThemeTag& getTheme(EnumType key);

    protected:
        // ── Registration ──────────────────────────────────────────────────────

        /** @brief Register an asset file path. */
        template<typename EnumType>
        static void set(EnumType key, const char* filepath);

        /** @brief Register a string configuration value. */
        template<typename EnumType>
        static void set(EnumType key, std::string value);

        /**
         * @brief Register a typed configuration value (int, float, struct, etc.).
         *
         * Excludes @c const @c char*, @c std::string, and @c ThemeTag-derived
         * types — those are handled by their own overloads.
         */
        template<typename EnumType, typename ValueType>
        static std::enable_if_t<
            !std::is_same_v<std::decay_t<ValueType>, const char*> &&
            !std::is_same_v<std::decay_t<ValueType>, std::string> &&
            !std::is_base_of_v<ThemeTag, std::decay_t<ValueType>>
        >
        set(EnumType key, ValueType&& value);

        /**
         * @brief Register a Theme-derived value.
         *
         * Stored as @c shared_ptr<ThemeTag> so @c Theme.h is not needed
         * in @c Manifest.tpp. @c ThemeManager::apply() casts back to @c Theme&.
         */
        template<typename EnumType, typename T>
        static std::enable_if_t<std::is_base_of_v<ThemeTag, std::decay_t<T>>>
        set(EnumType key, T&& value);

        /** @brief Register multiple key-value pairs in one call. */
        template<typename E, typename V, typename... Args>
        static void set(E key, V&& value, Args&&... args);

    private:
        template<typename Asset>
        static std::unordered_map<Asset, std::string, EnumClassHash>& getFilePaths();

        template<typename ConfigType, typename ValueType>
        static auto& getConfigs();

        template<typename EnumType>
        static auto& getThemes();
    };

} // namespace ml

#include "../../../src/Manifests/Manifest.tpp"
#endif // MANIFEST_H
