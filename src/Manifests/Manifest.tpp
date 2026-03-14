
#ifndef MANIFEST_TPP
#define MANIFEST_TPP
#include <Malena/Manifests/Manifest.h>
namespace ml
{
	 template<typename Asset>
     const std::string& Manifest::getFilepath(const Asset &asset)
     {
         return getFilePaths<Asset>()[asset];
     }

    template<typename ConfigType, typename ValueType>
    const ValueType& Manifest::getConfig(ConfigType config)
    {
        return getConfigs<ConfigType, ValueType>().at(config);
    }

    // Add asset - const char* means filepath
    template<typename EnumType>
    void Manifest::set(EnumType key, const char* filepath)
    {
        getFilePaths<EnumType>()[key] = std::string(filepath);
    }

    // Add string config - std::string explicitly means config value
    template<typename EnumType>
    void Manifest::set(EnumType key, std::string value)
    {
        getConfigs<EnumType, std::string>()[key] = std::move(value);
    }

    // Add config - other types (int, bool, float, etc.)
    template<typename EnumType, typename ValueType>
    std::enable_if_t<
        !std::is_same_v<std::decay_t<ValueType>, const char*> &&
        !std::is_same_v<std::decay_t<ValueType>, std::string>
    >
    Manifest::set(EnumType key, ValueType&& value)
    {
        getConfigs<EnumType, std::decay_t<ValueType>>()[key] = std::forward<ValueType>(value);
    }

    // Variadic add for batch registration
    template<typename E, typename V, typename... Args>
    void Manifest::set(E key, V&& value, Args&&... args)
    {
        set(key, std::forward<V>(value));
        if constexpr (sizeof...(args) > 0)
        {
            set(std::forward<Args>(args)...);
        }
    }

    template<typename Asset>
    std::unordered_map<Asset, std::string, EnumClassHash>& Manifest::getFilePaths()
    {
        static std::unordered_map<Asset, std::string, EnumClassHash> _filepaths;
        return _filepaths;
    }

    template<typename ConfigType, typename ValueType>
    auto& Manifest::getConfigs()
    {
        static std::unordered_map<ConfigType, ValueType, EnumClassHash> _configs;
        return _configs;
    }
}
#endif