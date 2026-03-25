// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef DEFAULTMANIFEST_H
#define DEFAULTMANIFEST_H

namespace ml
{
    /**
     * @brief Placeholder manifest used when no custom manifest is provided.
      * @ingroup Manifests
     *
     * @c DefaultManifest satisfies the manifest interface for template
     * parameters that require a manifest type but where the class has no
     * actual resources, flags, or states to declare. It provides a single
     * @c Asset::DUMMY enum value so that the @c ResourceManager template
     * instantiates without an empty enum.
     *
     * You will rarely reference @c DefaultManifest directly. It appears as
     * the default template argument in managers like @c FontManager:
     * @code
     * // Uses DefaultManifest — loads the built-in Arial font
     * const sf::Font& f = ml::FontManager<>::getDefault();
     * @endcode
     *
     * For classes that need resources or flags, declare a real manifest
     * and pass it to @c ComponentWith, @c PluginWith, etc.
     *
     * @see Manifest, FontManager
     */
    class DefaultManifest
    {
    public:
        /// Placeholder asset enum required by @c ResourceManager's template contract.
        enum class Asset { DUMMY };

        DefaultManifest() {}
    };

} // namespace ml

#endif // DEFAULTMANIFEST_H
