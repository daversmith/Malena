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
     * type to use the appropriate SFML loading API without virtual dispatch.
     *
     * @tparam Manifest       A @c Manifest subclass whose enum keys map to file paths.
     * @tparam Resource       The SFML resource type to load and cache.
     * @tparam loadFunction   Function pointer @c bool(Resource&, const std::string&).
     *
     * @see TextureManager, FontManager, SoundManager, AssetsManager, Manifest
     */
    template<typename Manifest,
             typename Resource,
             bool (*loadFunction)(Resource&, const std::string&) = &fileLoader>
    /**
     * @brief ResourceManager.
     * @ingroup Resources
     */
    class ResourceManager
    {
    public:
        /**
         * @brief Retrieve the cached resource for an asset enum value.
         *
         * Loads and caches on first access. Throws @c std::runtime_error
         * if the path is not registered or loading fails.
         *
         * @tparam Asset  An enum type declared inside @c Manifest.
         * @param  asset  The specific enum value to load.
         * @return Const reference to the cached @c Resource.
         */
        template<typename Asset>
        static const Resource& get(const Asset& asset);

        /**
         * @brief Remove a resource from the cache.
         *
         * The next @c get(asset) call will reload from disk.
         * If called during @c draw(), the unload is deferred until
         * after the frame completes to prevent mid-frame dangling references.
         *
         * @tparam Asset  The enum type of the key to remove.
         * @param  asset  The specific enum value to evict from the cache.
         */
        template<typename Asset>
        static void unload(Asset asset);

    private:
        /**
         * @brief Shared cache — used by both get() and unload().
         *
         * A static local inside a static function gives a single instance
         * per (Manifest, Resource, Asset) combination, accessible from
         * anywhere without a class-level static member.
         */
        template<typename Asset>
        static std::unordered_map<Asset, Resource, EnumClassHash>& cache();
    };

} // namespace ml

#include "../../../src/Resources/ResourceManager.tpp"
#endif // RESOURCEMANAGER_H