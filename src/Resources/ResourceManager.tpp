//
// Created by Dave Smith on 10/3/25.
//
#ifndef RESOURCEMANAGER_CPP
#define RESOURCEMANAGER_CPP

#include <Malena/Resources/ResourceManager.h>
#include <Malena/Engine/App/AppManager.h>
#include <iostream>

namespace ml
{
    // ── cache() ───────────────────────────────────────────────────────────────

    template<typename Manifest, typename Resource, bool (*loadFunction)(Resource&, const std::string&)>
    template<typename Asset>
    std::unordered_map<Asset, Resource, EnumClassHash>&
    ResourceManager<Manifest, Resource, loadFunction>::cache()
    {
        // Single static instance per (Manifest, Resource, Asset) combination.
        // Both get() and unload() reference this same map.
        static std::unordered_map<Asset, Resource, EnumClassHash> _cache;
        return _cache;
    }

    // ── get() ─────────────────────────────────────────────────────────────────

    template<typename Manifest, typename Resource, bool (*loadFunction)(Resource&, const std::string&)>
    template<typename Asset>
    const Resource& ResourceManager<Manifest, Resource, loadFunction>::get(const Asset& asset)
    {
        static_assert(
            std::is_enum<Asset>::value,
            "Asset must be an enum type declared in the Manifest."
        );

        static Manifest instance;
        auto& resources = cache<Asset>();

        auto it = resources.find(asset);
        if (it != resources.end())
            return it->second;

        Resource resource;
        if (!loadFunction(resource, Manifest::getFilepath(asset)))
            throw std::runtime_error("Failed to load resource");

        resources[asset] = std::move(resource);
        return resources[asset];
    }

    // ── unload() ──────────────────────────────────────────────────────────────

    template<typename Manifest, typename Resource, bool (*loadFunction)(Resource&, const std::string&)>
    template<typename Asset>
    void ResourceManager<Manifest, Resource, loadFunction>::unload(Asset asset)
    {
        static_assert(
            std::is_enum<Asset>::value,
            "Asset must be an enum type declared in the Manifest."
        );

        // Guard — do not unload while draw() is iterating components.
        // Defer to after the frame so no component is left with a
        // dangling resource reference mid-draw.
        if (AppManager::isDrawing())
        {
            std::cerr << "[Malena] Warning: Resources::unload() called during draw() "
                      << "— deferred to end of frame.\n";
            AppManager::deferUnload([asset, &c = cache<Asset>()]()
            {
                c.erase(asset);
            });
            return;
        }

        cache<Asset>().erase(asset);
    }

} // namespace ml

#endif
