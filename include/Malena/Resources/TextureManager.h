// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 4/8/25.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SFML/Graphics/Texture.hpp>
#include <Malena/Resources/ResourceManager.h>

namespace ml
{
    /**
     * @brief Manifest-driven cache for @c sf::Texture resources.
     *
     * @c TextureManager<Manifest> extends @c ResourceManager to provide
     * lazy-loaded, cached access to textures declared in a manifest's
     * @c Images enum.
     *
     * ### Usage
     * @code
     * // Direct access
     * const sf::Texture& bg = ml::TextureManager<MyManifest>::get(MyManifest::Images::Background);
     *
     * // Via AssetsManager (preferred when multiple asset types are needed)
     * const sf::Texture& bg = ml::AssetsManager<MyManifest>::get(MyManifest::Images::Background);
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with an @c Images enum whose
     *                  values map to texture file paths.
     *
     * @ingroup Resources
     * @see ResourceManager, AssetsManager, Manifest
     */
    template<typename Manifest>
    class TextureManager : public ml::ResourceManager<Manifest, sf::Texture>
    {};

} // namespace ml

#endif // TEXTUREMANAGER_H
