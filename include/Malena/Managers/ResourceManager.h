//
// Created by Dave Smith on 10/3/25.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <algorithm>
#include <unordered_map>
#include <Malena/Utilities/EnumClassHash.h>
#include <Malena/Utilities/LoaderFunctions.h>
namespace ml
{


    template <typename Manifest, typename Resource,  bool (*loadFunction)(Resource& r, const std::string& path) = &fileLoader>
    class ResourceManager {
    public:
        template<typename Asset>
        static const Resource& get(const Asset &asset);

        template<typename Asset>
        static void unload(Asset asset);
    };

}
#include "../../../src/Managers/ResourceManager.cpp"
#endif //RESOURCEMANAGER_H
