//
// Created by Dave Smith on 10/3/25.
//

#ifndef MANIFEST_H
#define MANIFEST_H

#include <unordered_map>
#include <string>
#include <type_traits>
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

        template<typename ConfigType, typename ValueType>
        static const ValueType& getConfig(ConfigType config)
        {
            return getConfigs<ConfigType, ValueType>().at(config);
        }

    protected:
        // Add asset - const char* means filepath
        template<typename EnumType>
        static void set(EnumType key, const char* filepath)
        {
            getFilePaths<EnumType>()[key] = std::string(filepath);
        }

        // Add string config - std::string explicitly means config value
        template<typename EnumType>
        static void set(EnumType key, std::string value)
        {
            getConfigs<EnumType, std::string>()[key] = std::move(value);
        }

        // Add config - other types (int, bool, float, etc.)
        template<typename EnumType, typename ValueType>
        static std::enable_if_t<
            !std::is_same_v<std::decay_t<ValueType>, const char*> &&
            !std::is_same_v<std::decay_t<ValueType>, std::string>
        >
        set(EnumType key, ValueType&& value)
        {
            getConfigs<EnumType, std::decay_t<ValueType>>()[key] = std::forward<ValueType>(value);
        }

        // Variadic add for batch registration
        template<typename E, typename V, typename... Args>
        static void set(E key, V&& value, Args&&... args)
        {
            set(key, std::forward<V>(value));
            if constexpr (sizeof...(args) > 0)
            {
                set(std::forward<Args>(args)...);
            }
        }

    private:
        template<typename Asset>
        static std::unordered_map<Asset, std::string, EnumClassHash>& getFilePaths()
        {
            static std::unordered_map<Asset, std::string, EnumClassHash> _filepaths;
            return _filepaths;
        }

        template<typename ConfigType, typename ValueType>
        static auto& getConfigs()
        {
            static std::unordered_map<ConfigType, ValueType, EnumClassHash> _configs;
            return _configs;
        }
    };
}

#endif //MANIFEST_H