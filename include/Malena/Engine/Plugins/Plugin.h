// Plugin.h
#pragma once

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT
#endif

#include <Malena/Traits/Base/Customizable.h>
#include <Malena/Core/Core.h>
#include <Malena/Core/Export.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/FireableHelper.h>
#include <Malena/Resources/FlagManager.h>
#include <Malena/Resources/TextureManager.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Utilities/Flag.h>
#include <SFML/Graphics/Texture.hpp>
#include <type_traits>

namespace ml
{
    /// @cond INTERNAL

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

    template<typename M, typename = void>
    struct HasThumbnail : std::false_type {};
    template<typename M>
    struct HasThumbnail<M, std::void_t<decltype(M::Images::THUMBNAIL)>>
        : std::true_type {};

    /// @endcond

    /**
     * @brief Abstract base class for all Malena plugins.
     * @ingroup EnginePlugins
     *
     * @c Plugin defines the lifecycle interface that every plugin must implement.
     * Subclass via @c PluginWith<Manifest> rather than @c Plugin directly —
     * @c PluginWith wires in the manifest, flag/state machinery, and resource
     * accessors automatically.
     *
     * ### Lifecycle
     * - @c onLoad() — called immediately after the plugin is constructed and
     *   registered. Use this to set up components, subscribe to events, etc.
     * - @c onUnload() — called just before the plugin is destroyed. Use this
     *   to clean up any state the plugin owns.
     *
     * ### Type queries
     * @code
     * // Check if a plugin implements a specific interface
     * if (plugin->is<IScorePlugin>()) { ... }
     *
     * // Get a typed pointer (returns nullptr if not that type)
     * if (auto* score = plugin->getIf<IScorePlugin>()) {
     *     score->addPoints(10);
     * }
     * @endcode
     *
     * @see PluginWith, PluginManager, ML_EXPORT
     */
    class Plugin : public Messenger
    {
    public:
        virtual ~Plugin() = default;

        /**
         * @brief Return the plugin's display name.
         * @return Null-terminated name string. Defaults to @c "Unnamed Plugin".
         */
        virtual const char* getName() const { return "Unnamed Plugin"; }

        /**
         * @brief Return the plugin's version string.
         * @return Null-terminated version string. Defaults to @c "1.0.0".
         */
        virtual const char* getVersion() const { return "1.0.0"; }

        /**
         * @brief Return a pointer to this plugin's thumbnail texture, or @c nullptr.
         *
         * Used by @c PluginManager::scanPlugins() to populate @c PluginInfo
         * records for display in a carousel or menu. Return @c nullptr if the
         * plugin has no thumbnail.
         *
         * @return Pointer to the thumbnail @c sf::Texture, or @c nullptr.
         */
        virtual const sf::Texture* getThumbnail() const { return nullptr; }

        /**
         * @brief Called immediately after the plugin is loaded and constructed.
         *
         * Override to initialise components, subscribe to events, load resources,
         * and perform any other startup work.
         */
        virtual void onLoad() {}

        /**
         * @brief Called just before the plugin is destroyed and unloaded.
         *
         * Override to clean up any state the plugin owns. All event and message
         * subscriptions are removed automatically after this call, so explicit
         * unsubscription is not required.
         */
        virtual void onUnload() {}

        /**
         * @brief Return @c true if this plugin is an instance of @c T.
         *
         * @tparam T The type to check against.
         */
        template<typename T>
        bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

        /**
         * @brief Return a @c T* if this plugin is an instance of @c T,
         *        or @c nullptr otherwise.
         *
         * @tparam T The type to cast to.
         */
        template<typename T>
        T* getIf() { return dynamic_cast<T*>(this); }

        /**
         * @brief Const overload of @c getIf.
         *
         * @tparam T The type to cast to.
         */
        template<typename T>
        const T* getIf() const { return dynamic_cast<const T*>(this); }

        /// @cond INTERNAL
        virtual Plugin* asPlugin() { return this; }
        /// @endcond
    };

    /// @cond INTERNAL

    template<typename... Traits>
    struct TraitsHaveCore : std::disjunction<
        std::is_base_of<Core, Traits>...
    > {};

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
        ml::Plugin* asPlugin()         override { return this; }

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
        ml::Plugin* asPlugin()         override { return this; }

        const sf::Texture* getThumbnail() const override
        {
            if constexpr (HasThumbnail<Manifest>::value)
                return &ml::TextureManager<Manifest>::get(Manifest::Images::THUMBNAIL);
            else
                return nullptr;
        }

        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

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

    /// @endcond

    /**
     * @brief Primary base class for manifest-driven plugins.
     * @ingroup EnginePlugins
     *
     * @c PluginWith<Manifest> is the recommended way to create a Malena plugin.
     * It wires in the manifest's resource enums, flag/state machinery, and
     * satisfies the @c Plugin interface automatically.
     *
     * @code
     * class MyPlugin : public ml::PluginWith<MyManifest>
     * {
     * public:
     *     void onLoad()   override { /* set up components *\/ }
     *     void onUnload() override { /* clean up *\/ }
     * };
     * ML_EXPORT(MyPlugin)
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with @c name and @c version
     *                  static constexpr members.
     * @tparam Traits   Optional extra traits (e.g. @c Messenger, @c Fadeable).
     * @see Plugin, PluginManager, ML_EXPORT
     */
    template<typename Manifest, typename... Traits>
    using PluginWith = std::conditional_t<
        TraitsHaveCore<Traits...>::value,
        PluginBaseWithCore<Manifest, Traits...>,
        PluginBase<Manifest, Traits...>
    >;

} // namespace ml

// =============================================================================
//  PluginHelper
// =============================================================================

namespace ml::exports
{
    /// @cond INTERNAL

    template<typename T, typename = void>
    struct PluginHelper
    {
        static ml::Plugin* create()             { return nullptr; }
        static void        destroy(ml::Plugin*) {}
    };

    template<typename T>
    struct PluginHelper<T, std::enable_if_t<std::is_base_of_v<ml::Plugin, T>>>
    {
        static ml::Plugin* create()               { return (new T())->asPlugin(); }
        static void        destroy(ml::Plugin* p) { delete p; }
    };

    /// @endcond
}

// =============================================================================
//  ML_EXPORT — full version (Plugin + Fireable both complete)
// =============================================================================

#undef ML_EXPORT

/**
 * @def ML_EXPORT(ClassName)
 * @brief Register a Malena type with the framework at program startup.
 * @ingroup EnginePlugins
 *
 * Place @c ML_EXPORT(ClassName) in the same header or translation unit as the
 * class definition. It handles two distinct registration paths:
 *
 * - **Plugin** — emits @c extern @c "C" factory symbols (@c createPlugin /
 *   @c destroyPlugin) so that @c PluginManager can load the class from a
 *   shared library via @c dlopen / @c LoadLibrary.
 * - **Fireable** — registers a singleton dispatcher instance with
 *   @c Fireable::_register so that @c AppManager dispatches events to it.
 * - **Plugin + Fireable** — the plugin path takes precedence; @c PluginManager
 *   registers the @c Fireable aspect at load time.
 * - **Neither** — a @c static_assert fires at compile time.
 *
 * @param ClassName The class to register. Must be fully defined at the point
 *                  where @c ML_EXPORT appears.
 *
 * @see PluginWith, Fireable, PluginManager
 */
#define ML_EXPORT(ClassName)                                                        \
                                                                                    \
    extern "C" {                                                                    \
        PLUGIN_EXPORT ml::Plugin* createPlugin()                                    \
        {                                                                           \
            return ml::exports::PluginHelper<ClassName>::create();                  \
        }                                                                           \
        PLUGIN_EXPORT void destroyPlugin(ml::Plugin* p)                             \
        {                                                                           \
            ml::exports::PluginHelper<ClassName>::destroy(p);                       \
        }                                                                           \
    }                                                                               \
                                                                                    \
    namespace ml::exports {                                                         \
        /** @cond INTERNAL */                                                       \
        inline struct _Register_##ClassName                                         \
        {                                                                           \
            _Register_##ClassName()                                                 \
            {                                                                       \
                ml::exports::FireableHelper<ClassName>::doRegister();               \
                                                                                    \
                static_assert(                                                      \
                    std::is_base_of_v<ml::Plugin,   ClassName> ||                  \
                    std::is_base_of_v<ml::Fireable, ClassName>,                    \
                    "[Malena] ML_EXPORT: '" #ClassName "' is not a "               \
                    "recognised exportable Malena type. "                           \
                    "Must derive from ml::Plugin or ml::Fireable.");                \
            }                                                                       \
        } _instance_##ClassName;                                                    \
        /** @endcond */                                                             \
    }
