//
// Created by Dave Smith on 10/3/25.
//
#ifndef RESOURCEMANAGER_CPP
#define RESOURCEMANAGER_CPP
#include <Malena/Managers/ResourceManager.h>

namespace ml
{
    template<typename Manifest, typename Resource, bool (*loadFunction)(Resource& r, const std::string& path)>
    template<typename Asset>
    const Resource& ResourceManager<Manifest, Resource, loadFunction>::get(const Asset &asset) {
        //Verify there is an Asset enum in the class

        static_assert(
            std::is_enum<Asset>::value,
            "Manifest must have a nested enum class called 'Asset'"
        );

        static std::unordered_map<Asset,Resource, EnumClassHash> resources;
        static Manifest instance;

        auto it = resources.find(asset);
        if (it != resources.end()) {
            return it->second;
        }

        Resource resource;

        if (!loadFunction(resource, Manifest::getFilepath(asset))){
            throw std::runtime_error("Failed to load resource");
        }

        resources[asset] = std::move(resource);
        return resources[asset];
    }
    template<typename Manifest, typename Resource, bool (*loadFunction)(Resource& r, const std::string& path)>
    template<typename Asset>
    void ResourceManager<Manifest, Resource, loadFunction>::unload(Asset asset)
    {
        //Verify there is a Asset enum in the class
        static_assert(
            std::is_enum<Asset>::value,
            "Manifest must have a nested enum class called 'Asset'"
        );
        // Verify getFilepath exists
        static_assert(
            std::is_same_v<
                decltype(Manifest::getFilepath(asset)),
                std::string
            >,
            "Manifest must have a static getFilepath(Image) method returning std::string"
        );

        static std::unordered_map<Asset, Resource, EnumClassHash> resources;
        resources.erase(asset);
    }
}
#endif