
#ifndef MANIFEST_TPP
#define MANIFEST_TPP
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/ThemeTag.h>

namespace ml
{
    template<typename Asset>
    const std::string& Manifest::getFilepath(const Asset& asset)
    {
        return getFilePaths<Asset>().at(asset);
    }

    template<typename ConfigType, typename ValueType>
    const ValueType& Manifest::getConfig(ConfigType config)
    {
        return getConfigs<ConfigType, ValueType>().at(config);
    }

    template<typename EnumType>
    const ThemeTag& Manifest::getTheme(EnumType key)
    {
        return *getThemes<EnumType>().at(key);
    }

    // ── filepath ──────────────────────────────────────────────────────────────

    template<typename EnumType>
    void Manifest::set(EnumType key, const char* filepath)
    {
        getFilePaths<EnumType>()[key] = std::string(filepath);
    }

    // ── std::string config ────────────────────────────────────────────────────

    template<typename EnumType>
    void Manifest::set(EnumType key, std::string value)
    {
        getConfigs<EnumType, std::string>()[key] = std::move(value);
    }

    // ── generic config (int, float, struct, etc.) — excludes ThemeTag types ──

    template<typename EnumType, typename ValueType>
    std::enable_if_t<
        !std::is_same_v<std::decay_t<ValueType>, const char*> &&
        !std::is_same_v<std::decay_t<ValueType>, std::string> &&
        !std::is_base_of_v<ThemeTag, std::decay_t<ValueType>>
    >
    Manifest::set(EnumType key, ValueType&& value)
    {
        getConfigs<EnumType, std::decay_t<ValueType>>()[key] = std::forward<ValueType>(value);
    }

    // ── Theme-derived — stored as shared_ptr<ThemeTag>, no Theme.h needed ────

    template<typename EnumType, typename T>
    std::enable_if_t<std::is_base_of_v<ThemeTag, std::decay_t<T>>>
    Manifest::set(EnumType key, T&& value)
    {
        getThemes<EnumType>()[key] =
            std::make_shared<std::decay_t<T>>(std::forward<T>(value));
    }

    // ── variadic batch registration ───────────────────────────────────────────

    template<typename E, typename V, typename... Args>
    void Manifest::set(E key, V&& value, Args&&... args)
    {
        set(key, std::forward<V>(value));
        if constexpr (sizeof...(args) > 0)
            set(std::forward<Args>(args)...);
    }

    // ── storage ───────────────────────────────────────────────────────────────

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

    template<typename EnumType>
    auto& Manifest::getThemes()
    {
        static std::unordered_map<EnumType,
            std::shared_ptr<ThemeTag>, EnumClassHash> _themes;
        return _themes;
    }
}
#endif
