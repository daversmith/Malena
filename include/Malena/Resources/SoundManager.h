// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <Malena/Resources/ResourceManager.h>
#include <SFML/Audio/SoundBuffer.hpp>

namespace ml
{
    /**
     * @brief Manifest-driven cache for @c sf::SoundBuffer resources.
     *
     * @c SoundManager<Manifest> extends @c ResourceManager to provide
     * lazy-loaded, cached access to sound buffers declared in a manifest's
     * @c Sounds enum.
     *
     * The cached @c sf::SoundBuffer can be assigned to an @c sf::Sound
     * instance for playback. The buffer remains valid for the lifetime of
     * the application (or until @c unload() is called).
     *
     * ### Usage
     * @code
     * // Direct access
     * const sf::SoundBuffer& buf =
     *     ml::SoundManager<MyManifest>::get(MyManifest::Sounds::Jump);
     *
     * sf::Sound sound;
     * sound.setBuffer(buf);
     * sound.play();
     *
     * // Via AssetsManager (preferred when multiple asset types are needed)
     * const sf::SoundBuffer& buf =
     *     ml::AssetsManager<MyManifest>::get(MyManifest::Sounds::Jump);
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with a @c Sounds enum whose
     *                  values map to audio file paths.
     *
     * @see ResourceManager, AssetsManager, Manifest
     */
    template<typename Manifest>
    /**
     * @brief SoundManager.
     * @ingroup Resources
     */
    class SoundManager : public ml::ResourceManager<Manifest, sf::SoundBuffer>
    {};

} // namespace ml

#endif // SOUNDMANAGER_H
