// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// ManifestResources.h
//

#ifndef RESOURCES_H
#define RESOURCES_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Resources/AssetsManager.h>
#include <Malena/Resources/ConfigManager.h>
#include <Malena/Manifests/ManifestAliases.h>
#include <type_traits>

namespace ml
{
    // ── Per-type asset enum detection ─────────────────────────────────────────

    /// @cond INTERNAL

    template<typename Manifest, typename EnumType, typename = void>
    struct is_images_enum : std::false_type {};
    template<typename Manifest, typename EnumType>
    struct is_images_enum<Manifest, EnumType,
        std::void_t<typename Manifest::Images>>
        : std::bool_constant<
            std::is_same_v<EnumType, typename Manifest::Images>> {};

    template<typename Manifest, typename EnumType, typename = void>
    struct is_fonts_enum : std::false_type {};
    template<typename Manifest, typename EnumType>
    struct is_fonts_enum<Manifest, EnumType,
        std::void_t<typename Manifest::Fonts>>
        : std::bool_constant<
            std::is_same_v<EnumType, typename Manifest::Fonts>> {};

    template<typename Manifest, typename EnumType, typename = void>
    struct is_sounds_enum : std::false_type {};
    template<typename Manifest, typename EnumType>
    struct is_sounds_enum<Manifest, EnumType,
        std::void_t<typename Manifest::Sounds>>
        : std::bool_constant<
            std::is_same_v<EnumType, typename Manifest::Sounds>> {};

    template<typename Manifest, typename EnumType>
    struct is_asset_enum : std::bool_constant<
        is_images_enum<Manifest, EnumType>::value ||
        is_fonts_enum <Manifest, EnumType>::value ||
        is_sounds_enum<Manifest, EnumType>::value
    > {};

    // Builds const T& from any type — strips ref, adds const, adds lvalue ref
    template<typename T>
    using const_ref_t = std::add_lvalue_reference_t<
                            std::add_const_t<
                                std::remove_reference_t<T>>>;

    /// @endcond

    /**
     * @brief Unified manifest resource and config accessor.
     * @ingroup Manifests
     *
     * @c ManifestResources<Manifest> provides a single @c get() that routes
     * to the correct manager based on the enum type:
     *
     * - Asset enums (@c Images, @c Fonts, @c Sounds) → @c AssetsManager
     * - Config enums (@c Text, @c Ints, @c Floats, @c Booleans) → @c ConfigManager
     *
     * Asset overloads always return @c const @c T& — so taking the address
     * with @c & always yields @c const @c T* with no type deduction ambiguity:
     *
     * @code
     * class MyComponent : public ml::ComponentWith<MyManifest>
     * {
     *     void onInit() override
     *     {
     *         // Reference — for auto& binding and direct use
     *         auto& tex  = Resources::get(Images::Background);
     *         auto& font = Resources::get(Fonts::Main);
     *
     *         // Pointer — works directly, no intermediate variable needed
     *         circle.setTexture(&Resources::get(Images::Icon));
     *         sprite.setTexture(&Resources::get(Images::Background));
     *
     *         // Config
     *         auto& title = Resources::get(Text::WindowTitle);
     *         int   count = Resources::get(Ints::MaxPlayers);
     *     }
     * };
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass.
     * @see ManifestAliases, AssetsManager, ConfigManager, ComponentWith
     */
    template<typename TManifest>
    struct ManifestResources : public TManifest
    {
        /**
         * @brief Retrieve an asset by enum key → @c AssetsManager.
         *
         * Return type is explicitly @c const @c T& — taking @c & gives
         * @c const @c T* with no ambiguity, suitable for SFML functions
         * that take @c const @c sf::Texture* etc.
         */
        template<typename EnumType>
        static auto get(EnumType key)
            -> std::enable_if_t<
                is_asset_enum<TManifest, EnumType>::value,
                const_ref_t<decltype(AssetsManager<TManifest>::get(key))>>
        {
            return AssetsManager<TManifest>::get(key);
        }

        /**
         * @brief Retrieve a config value by enum key → @c ConfigManager.
         *
         * Return type is inferred from @c ConfigManager::get() —
         * @c Text gives @c const @c std::string&, @c Ints gives @c int, etc.
         */
        template<typename EnumType>
        static auto get(EnumType key)
            -> std::enable_if_t<
                !is_asset_enum<TManifest, EnumType>::value,
                decltype(ConfigManager<TManifest>::get(key))>
        {
            return ConfigManager<TManifest>::get(key);
        }

    	template<typename EnumType>
		static std::enable_if_t<is_images_enum<TManifest, EnumType>::value>
		unload(EnumType key)
        {
        	TextureManager<TManifest>::unload(key);
        }

    	template<typename EnumType>
		static std::enable_if_t<is_fonts_enum<TManifest, EnumType>::value>
		unload(EnumType key)
        {
        	FontManager<TManifest>::unload(key);
        }

    	template<typename EnumType>
		static std::enable_if_t<is_sounds_enum<TManifest, EnumType>::value>
		unload(EnumType key)
        {
        	SoundManager<TManifest>::unload(key);
        }
    };

} // namespace ml

#endif // RESOURCES_H