// Plugin.h
#pragma once

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT
#endif

#include <Malena/Traits/Customizable.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Traits/MultiCustomFlaggable.h>
#include <Malena/Traits/MultiCustomStateManager.h>
#include <Malena/Managers/FlagManager.h>
#include <Malena/Managers/TextureManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Interfaces/Core.h>
#include <SFML/Graphics/Texture.hpp>
#include <type_traits>

namespace ml {

    // ── Manifest validation traits (C++17) ───────────────────────────────────

    template<typename M, typename = void>
    struct HasPluginName : std::false_type {};

    template<typename M>
    struct HasPluginName<M, std::void_t<decltype(M::name)>>
        : std::is_convertible<decltype(M::name), const char*> {};

    template<typename M, typename = void>
    struct HasPluginVersion : std::false_type {};

    template<typename M>
    struct HasPluginVersion<M, std::void_t<decltype(M::version)>>
        : std::is_convertible<decltype(M::version), const char*> {};

    // Detects whether the manifest declares Images::THUMBNAIL
    template<typename M, typename = void>
    struct HasThumbnail : std::false_type {};

    template<typename M>
    struct HasThumbnail<M, std::void_t<decltype(M::Images::THUMBNAIL)>>
        : std::true_type {};

    // ── Plugin base ───────────────────────────────────────────────────────────

    class Plugin : public Messenger
    {
    public:
        virtual ~Plugin() = default;
        virtual const char*        getName()      const { return "Unnamed Plugin"; }
        virtual const char*        getVersion()   const { return "1.0.0"; }
        virtual const sf::Texture* getThumbnail() const { return nullptr; }
        virtual void onLoad()   {}
        virtual void onUnload() {}
        virtual Plugin* asPlugin() { return this; }

        template<typename T>
        bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

        template<typename T>
        T* getIf() { return dynamic_cast<T*>(this); }

        template<typename T>
        const T* getIf() const { return dynamic_cast<const T*>(this); }
    };

    // Detect whether any trait brings Core into the hierarchy
    template<typename... Traits>
    struct TraitsHaveCore : std::disjunction<
        std::is_base_of<Core, Traits>...
    > {};

    // ── PluginBase — no Core in traits ────────────────────────────────────────
    template<typename Manifest, typename... Traits>
    struct PluginBase : public Plugin,
                        public Customizable<Manifest>,
                        public Traits...,
                        public GatherFlags <Manifest, Traits...>::type,
                        public GatherStates<Manifest, Traits...>::type
    {
        static_assert(HasPluginName<Manifest>::value,
            "[Malena] Manifest is missing:  static constexpr const char* name = \"Your Plugin Name\";");
        static_assert(HasPluginVersion<Manifest>::value,
            "[Malena] Manifest is missing:  static constexpr const char* version = \"1.0.0\";");

        using manifest_type = Manifest;

        const char* getName()    const override { return Manifest::name; }
        const char* getVersion() const override { return Manifest::version; }

        // Satisfies both ml::Plugin::asPlugin() and ds::GamePlugin::asPlugin() = 0
        // when this class is combined with ds::GamePlugin in a concrete plugin class.
        ml::Plugin* asPlugin() override { return this; }

        // Automatically returns the manifest thumbnail if Images::THUMBNAIL exists,
        // otherwise returns nullptr — no code needed in the plugin class itself.
        const sf::Texture* getThumbnail() const override
        {
            if constexpr (HasThumbnail<Manifest>::value)
                return &ml::TextureManager<Manifest>::get(Manifest::Images::THUMBNAIL);
            else
                return nullptr;
        }

        using GatherFlags<Manifest, Traits...>::type::enableFlag;
        using GatherFlags<Manifest, Traits...>::type::disableFlag;
        using GatherFlags<Manifest, Traits...>::type::checkFlag;
        using GatherFlags<Manifest, Traits...>::type::setFlag;
        using GatherFlags<Manifest, Traits...>::type::toggleFlag;

        using GatherStates<Manifest, Traits...>::type::setState;
        using GatherStates<Manifest, Traits...>::type::getState;
        using GatherStates<Manifest, Traits...>::type::isState;
        using GatherStates<Manifest, Traits...>::type::onStateEnter;
        using GatherStates<Manifest, Traits...>::type::onStateExit;
    };

    // ── PluginBaseWithCore — Core present via traits ───────────────────────────
    template<typename Manifest, typename... Traits>
    struct PluginBaseWithCore : public Plugin,
                                public Customizable<Manifest>,
                                public Traits...,
                                public GatherFlags <Manifest, Traits...>::type,
                                public GatherStates<Manifest, Traits...>::type
    {
        static_assert(HasPluginName<Manifest>::value,
            "[Malena] Manifest is missing:  static constexpr const char* name = \"Your Plugin Name\";");
        static_assert(HasPluginVersion<Manifest>::value,
            "[Malena] Manifest is missing:  static constexpr const char* version = \"1.0.0\";");

        using manifest_type = Manifest;

        const char* getName()    const override { return Manifest::name; }
        const char* getVersion() const override { return Manifest::version; }

        ml::Plugin* asPlugin() override { return this; }

        const sf::Texture* getThumbnail() const override
        {
            if constexpr (HasThumbnail<Manifest>::value)
                return &ml::TextureManager<Manifest>::get(Manifest::Images::THUMBNAIL);
            else
                return nullptr;
        }

        // System flags
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // Custom flags
        using GatherFlags<Manifest, Traits...>::type::enableFlag;
        using GatherFlags<Manifest, Traits...>::type::disableFlag;
        using GatherFlags<Manifest, Traits...>::type::checkFlag;
        using GatherFlags<Manifest, Traits...>::type::setFlag;
        using GatherFlags<Manifest, Traits...>::type::toggleFlag;

        // Custom states
        using GatherStates<Manifest, Traits...>::type::setState;
        using GatherStates<Manifest, Traits...>::type::getState;
        using GatherStates<Manifest, Traits...>::type::isState;
        using GatherStates<Manifest, Traits...>::type::onStateEnter;
        using GatherStates<Manifest, Traits...>::type::onStateExit;
    };

    /**
     * @brief Plugin with manifest support.
     *
     * Manifest must declare name and version:
     * @code
     * class MyManifest : public ml::Manifest {
     * public:
     *     static constexpr const char* name    = "My Plugin";
     *     static constexpr const char* version = "1.0.0";
     *
     *     // Optional — adds thumbnail support automatically:
     *     enum class Images { THUMBNAIL };
     *
     *     enum class Flag  { ... };
     *     enum class State { ... };
     * private:
     *     inline static const auto _ = [](){
     *         set(Images::THUMBNAIL, "assets/thumb.png");
     *         return 0;
     *     }();
     * };
     * @endcode
     */
    template<typename Manifest, typename... Traits>
    using PluginWith = std::conditional_t<
        TraitsHaveCore<Traits...>::value,
        PluginBaseWithCore<Manifest, Traits...>,
        PluginBase<Manifest, Traits...>
    >;

} // namespace ml

// ── REGISTER_PLUGIN ───────────────────────────────────────────────────────────
//
//   REGISTER_PLUGIN(MyGame)
//       — reads name/version from manifest automatically
//
//   REGISTER_PLUGIN(MyGame, "My Game", "1.0.0")
//       — explicit override, use for plugins without PluginWith
//
#define REGISTER_PLUGIN_1(GameClass) \
extern "C" { \
PLUGIN_EXPORT ml::Plugin* createPlugin() \
    { return (new GameClass())->asPlugin(); } \
PLUGIN_EXPORT void destroyPlugin(ml::Plugin* p) \
    { delete p; } \
}

#define REGISTER_PLUGIN_3(GameClass, name, version) \
extern "C" { \
PLUGIN_EXPORT ml::Plugin* createPlugin() \
    { return (new GameClass())->asPlugin(); } \
PLUGIN_EXPORT void destroyPlugin(ml::Plugin* p) \
    { delete p; } \
}

#define REGISTER_PLUGIN_PICK(_1, _2, _3, NAME, ...) NAME
#define REGISTER_PLUGIN(...) \
    REGISTER_PLUGIN_PICK(__VA_ARGS__, REGISTER_PLUGIN_3, _UNUSED, REGISTER_PLUGIN_1)(__VA_ARGS__)