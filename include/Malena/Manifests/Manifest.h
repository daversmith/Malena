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
    /**
     * @brief Base class for all Malena manifests.
     *
     * A manifest is a static registry that maps enum keys to file paths or
     * configuration values. Subclasses declare inner enum types (@c Images,
     * @c Fonts, @c Sounds, @c Text, @c Ints, etc.) and populate them using
     * the protected @c set() methods, typically in an inline static initializer.
     *
     * The manifest is the contract between a class and its resources. By
     * declaring what a class needs in its manifest, the framework's template
     * machinery (@c AssetsManager, @c ConfigManager, @c ResourceManager) can
     * load and cache those resources automatically without any manual loading
     * code in the class body.
     *
     * ### Manifest structure
     * @code
     * class MyManifest : public ml::Manifest
     * {
     * public:
     *     // Required for plugins
     *     static constexpr const char* name    = "My Plugin";
     *     static constexpr const char* version = "1.0.0";
     *
     *     // Asset enums
     *     enum class Images  { Background, Button };
     *     enum class Fonts   { Main };
     *     enum class Sounds  { Click };
     *
     *     // Config enums
     *     enum class Text    { WelcomeMessage };
     *     enum class Ints    { MaxPlayers };
     *
     *     // Component-level flags and states
     *     enum class Flag    { Selected };
     *     enum class State   { Idle, Active };
     *
     * private:
     *     // Inline static initializer registers all paths at program startup
     *     inline static const auto _ = [](){
     *         set(Images::Background, "assets/bg.png");
     *         set(Images::Button,     "assets/btn.png");
     *         set(Fonts::Main,        "assets/main.ttf");
     *         set(Sounds::Click,      "assets/click.wav");
     *         set(Text::WelcomeMessage, std::string("Hello!"));
     *         set(Ints::MaxPlayers, 4);
     *         return 0;
     *     }();
     * };
     * @endcode
     *
     * ### Accessing registered values
     * @code
     * const sf::Texture& bg  = ml::TextureManager<MyManifest>::get(MyManifest::Images::Background);
     * const sf::Font&    fnt = ml::FontManager<MyManifest>::get(MyManifest::Fonts::Main);
     * const std::string& msg = ml::Manifest::getConfig<MyManifest::Text, std::string>(MyManifest::Text::WelcomeMessage);
     * @endcode
     *
     * Or more conveniently through @c AssetsManager or @c Context:
     * @code
     * ml::AssetsManager<MyManifest>::get(MyManifest::Images::Background);
     * @endcode
     *
     * @see AssetsManager, Context, Resources, ResourceManager, ConfigManager
     */
    class Manifest
    {
    public:
        /**
         * @brief Retrieve the file path registered for an asset enum value.
         *
         * Looks up the path previously registered via @c set(key, filepath).
         * Throws @c std::out_of_range if the key has not been registered.
         *
         * @tparam Asset  An enum type declared inside a @c Manifest subclass.
         * @param  asset  The specific enum value to look up.
         * @return Const reference to the registered file path string.
         */
        template<typename Asset>
        static const std::string& getFilepath(const Asset& asset);

        /**
         * @brief Retrieve a typed configuration value by enum key.
         *
         * Looks up the value previously registered via @c set(key, value)
         * where @c value was not a file path. Throws if the key has not
         * been registered.
         *
         * @tparam ConfigType  An enum type declared inside a @c Manifest subclass.
         * @tparam ValueType   The type of the stored value (e.g., @c int, @c float,
         *                     @c std::string).
         * @param  config      The specific enum value to look up.
         * @return Const reference to the registered configuration value.
         */
        template<typename ConfigType, typename ValueType>
        static const ValueType& getConfig(ConfigType config);

    protected:
        /**
         * @brief Register an asset file path.
         *
         * Associates @p key with a file path. The path is later retrieved
         * by @c ResourceManager when loading the asset.
         *
         * @tparam EnumType  Enum type of the key (inferred).
         * @param  key       The enum value to register.
         * @param  filepath  Null-terminated path to the asset file.
         */
        template<typename EnumType>
        static void set(EnumType key, const char* filepath);

        /**
         * @brief Register a string configuration value.
         *
         * Associates @p key with a @c std::string config value. Distinct
         * from the file-path overload — an explicit @c std::string signals
         * that this is configuration data, not a resource path.
         *
         * @tparam EnumType  Enum type of the key (inferred).
         * @param  key       The enum value to register.
         * @param  value     The string configuration value.
         */
        template<typename EnumType>
        static void set(EnumType key, std::string value);

        /**
         * @brief Register a typed configuration value (int, float, bool, etc.).
         *
         * Enabled for any @c ValueType that is not @c const @c char* or
         * @c std::string, routing numeric and boolean config values into their
         * own type-indexed storage.
         *
         * @tparam EnumType   Enum type of the key (inferred).
         * @tparam ValueType  Type of the configuration value (inferred).
         * @param  key        The enum value to register.
         * @param  value      The configuration value.
         */
        template<typename EnumType, typename ValueType>
        static std::enable_if_t<
            !std::is_same_v<std::decay_t<ValueType>, const char*> &&
            !std::is_same_v<std::decay_t<ValueType>, std::string>
        >
        set(EnumType key, ValueType&& value);

        /**
         * @brief Register multiple key-value pairs in one call.
         *
         * Variadic overload that expands to successive @c set() calls.
         * Useful for batch registration in the inline static initializer:
         * @code
         * set(Images::Background, "assets/bg.png",
         *     Images::Button,     "assets/btn.png",
         *     Fonts::Main,        "assets/main.ttf");
         * @endcode
         *
         * @tparam E     First key type.
         * @tparam V     First value type.
         * @tparam Args  Remaining key-value pairs.
         */
        template<typename E, typename V, typename... Args>
        static void set(E key, V&& value, Args&&... args);

    private:
        template<typename Asset>
        static std::unordered_map<Asset, std::string, EnumClassHash>& getFilePaths();

        template<typename ConfigType, typename ValueType>
        static auto& getConfigs();
    };

} // namespace ml

#include "../../../src/Manifests/Manifest.tpp"
#endif // MANIFEST_H
