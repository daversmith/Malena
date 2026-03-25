// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/5/25.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include <Malena/Resources/AssetsManager.h>
#include <Malena/Resources/ConfigManager.h>
#include <Malena/Resources/StateManager.h>
#include <Malena/Manifests/ManifestAliases.h>

namespace ml
{
    /**
     * @brief Combines @c AssetsManager, @c ConfigManager, and @c StateManager
     *        for a manifest into one convenience struct.
     * @ingroup Manifests
     *
     * @c Context<Manifest> is the highest-level manifest grouping. It also
     * inherits @c ManifestAliases so that inner enum types (@c Images,
     * @c Fonts, @c Sounds, @c State, @c Flags) are accessible without
     * full qualification.
     *
     * ### When to use @c Context
     * Use @c Context when a class or plugin needs @b both media assets
     * (textures, fonts, sounds) @b and configuration values (strings, ints,
     * floats, bools):
     *
     * @code
     * struct MyContext : public ml::Context<MyManifest>
     * {
     *     void setup()
     *     {
     *         // Assets
     *         auto& tex = AssetMgr::get(Images::Background);
     *         auto& fnt = AssetMgr::get(Fonts::Main);
     *
     *         // Config
     *         auto& title = ConfigMgr::get(MyManifest::Text::WindowTitle);
     *     }
     * };
     * @endcode
     *
     * ### Choosing the right level
     *
     * | You need | Use |
     * |----------|-----|
     * | One asset type only | @c TextureManager / @c FontManager / @c SoundManager directly |
     * | Multiple media types | @c AssetsManager<Manifest> |
     * | Media + configuration | @c Context<Manifest> (this struct) |
     *
     * @tparam Manifest A @c Manifest subclass declaring the resource and
     *                  config enums.
     *
     * @see AssetsManager, ConfigManager, StateManager, ManifestAliases
     */
    template<typename Manifest>
    struct Context : public ManifestAliases<Manifest>
    {
        /// @c AssetsManager typed to this manifest — access textures, fonts, sounds.
        using AssetMgr  = AssetsManager<Manifest>;

        /// @c ConfigManager typed to this manifest — access string/int/float/bool config.
        using ConfigMgr = ConfigManager<Manifest>;

        /// @c StateManager typed to this manifest — access the manifest's @c State enum.
        using StateMgr  = StateManager<Manifest>;
    };

} // namespace ml

#endif // CONTEXT_H
