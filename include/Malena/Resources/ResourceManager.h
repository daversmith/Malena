//
// Created by Dave Smith on 10/3/25.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <algorithm>
#include <unordered_map>
#include <Malena/Utilities/EnumClassHash.h>
#include <Malena/Utilities/LoaderFunctions.h>
#include <string>
#include <stdexcept>

namespace ml
{
    /**
     * @brief Generic manifest-driven resource cache.
     *
     * @c ResourceManager<Manifest, Resource, loadFunction> is the base for all
     * Malena asset managers. It maps enum keys (declared in @c Manifest) to
     * loaded @c Resource instances, loading each resource lazily on first
     * access and caching it for subsequent calls.
     *
     * The load function is a compile-time parameter, allowing each resource
     * type to use the appropriate SFML loading API (e.g., @c sf::Texture::loadFromFile
     * vs @c sf::Font::openFromFile) without virtual dispatch.
     *
     * ### How it works
     * 1. @c Manifest::set(key, "path/to/file") registers the file path at
     *    program startup (typically in an inline static initializer).
     * 2. On the first call to @c get(key), @c ResourceManager looks up the
     *    path via @c Manifest::getFilepath(key), calls @c loadFunction to
     *    populate a @c Resource object, stores it in a static cache, and
     *    returns a const reference.
     * 3. Subsequent calls to @c get(key) return the cached reference
     *    immediately.
     *
     * ### Concrete subclasses
     * Do not instantiate @c ResourceManager directly. Use the typed subclasses:
     *
     * | Subclass | Resource type |
     * |----------|--------------|
     * | @c TextureManager<Manifest> | @c sf::Texture |
     * | @c FontManager<Manifest>   | @c sf::Font    |
     * | @c SoundManager<Manifest>  | @c sf::SoundBuffer |
     *
     * Or access all three through @c AssetsManager<Manifest>::get().
     *
     * @tparam Manifest       A @c Manifest subclass whose enum keys map to file paths.
     * @tparam Resource       The SFML resource type to load and cache.
     * @tparam loadFunction   Function pointer @c bool(Resource&, const std::string&)
     *                        that loads the resource from a file path.
     *                        Defaults to @c fileLoader.
     *
     * @see TextureManager, FontManager, SoundManager, AssetsManager, Manifest
     */
    template<typename Manifest,
             typename Resource,
             bool (*loadFunction)(Resource&, const std::string&) = &fileLoader>
    class ResourceManager
    {
    public:
        /**
         * @brief Retrieve the cached resource for an asset enum value.
         *
         * If the resource for @p asset has not been loaded yet, it is loaded
         * now using the registered file path and @c loadFunction. Throws
         * @c std::runtime_error if the path is not registered or loading fails.
         *
         * @tparam Asset  An enum type declared inside @c Manifest (e.g.,
         *                @c Manifest::Images, @c Manifest::Fonts).
         * @param  asset  The specific enum value to load.
         * @return Const reference to the cached @c Resource.
         */
        template<typename Asset>
        static const Resource& get(const Asset& asset);

        /**
         * @brief Remove a resource from the cache.
         *
         * After this call, the next @c get(asset) will reload the resource
         * from disk. Use to free GPU or memory when an asset is no longer
         * needed.
         *
         * @tparam Asset  The enum type of the key to remove.
         * @param  asset  The specific enum value to evict from the cache.
         */
        template<typename Asset>
        static void unload(Asset asset);
    };

} // namespace ml

#include "../../../src/Resources/ResourceManager.tpp"
#endif // RESOURCEMANAGER_H
