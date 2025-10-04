//
// Created by Dave Smith on 10/3/25.
//

#ifndef MANIFEST_H
#define MANIFEST_H

#include <unordered_map>
#include <string>
#include <Malena/Utilities/EnumClassHash.h>
namespace ml
{

    class Manifest
    {

    public:

        template<typename Asset>
        static const std::string& getFilepath(const Asset &asset)
        {
            return getFilePaths<Asset>()[asset];
        }
    private:

        template<typename Asset>
        static std::unordered_map<Asset, std::string, EnumClassHash>& getFilePaths()
        {
            static std::unordered_map<Asset, std::string, EnumClassHash> _filepaths;
            return _filepaths;
        }

    protected:

        template<typename Asset>
        static void addAsset(Asset asset, const std::string& filepath)
        {
            getFilePaths<Asset>()[asset] = filepath;
        }
        template<typename E, typename... Args>
        static void register_assets(E asset, const char* path, Args&&... args) {
            addAsset(asset, std::string(path));
            if constexpr (sizeof...(args) > 0) {
                register_assets(std::forward<Args>(args)...);
            }
        }
    };
}
#endif //MANIFEST_H
