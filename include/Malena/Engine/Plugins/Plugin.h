// Plugin.h
#pragma once

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT
#endif

#include <Malena/Traits/Customizable.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Resources/FlagManager.h>
#include <Malena/Resources/TextureManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Core/Core.h>
#include <SFML/Graphics/Texture.hpp>
#include <type_traits>

namespace ml
{
    // =========================================================================
    // Manifest validation traits
    // =========================================================================

    /// @cond INTERNAL

    /** @brief Detects @c static @c constexpr @c const @c char* @c name on a manifest. */
    template<typename M, typename = void>
    struct HasPluginName : std::false_type {};
    template<typename M>
    struct HasPluginName<M, std::void_t<decltype(M::name)>>
        : std::is_convertible<decltype(M::name), const char*> {};

    /** @brief Detects @c static @c constexpr @c const @c char* @c version on a manifest. */
    template<typename M, typename = void>
    struct HasPluginVersion : std::false_type {};
    template<typename M>
    struct HasPluginVersion<M, std::void_t<decltype(M::version)>>
        : std::is_convertible<decltype(M::version), const char*> {};

    /** @brief Detects @c Images::THUMBNAIL on a manifest, enabling auto-thumbnail. */
    template<typename M, typename = void>
    struct HasThumbnail : std::false_type {};
    template<typename M>
    struct HasThumbnail<M, std::void_t<decltype(M::Images::THUMBNAIL)>>
        : std::true_type {};

    /// @endcond

    // =========================================================================
    // Plugin — virtual base class
    // =========================================================================

    /**
     * @brief Abstract base class for all Malena plugins.
     *
     * @c Plugin defines the lifecycle interface and identity methods that
     * @c PluginManager needs to load, query, and unload any plugin regardless
     * of its concrete type. It inherits @c Messenger so that plugins can
     * participate in typed message passing out of the box.
     *
     * ### Lifecycle hooks
     * - @c onLoad()   — called by @c PluginManager immediately after the plugin
     *                   is constructed. Initialize resources here.
     * - @c onUnload() — called by @c PluginManager immediately before the plugin
     *                   is destroyed. Release resources and unsubscribe here.
     *
     * ### Interface queries
     * Because plugins cross dylib boundaries, @c dynamic_cast between
     * unrelated plugin types is unreliable. Use the provided helpers instead:
     * @code
     * if (plugin->is<ml::UIComponent>()) { ... }
     *
     * if (auto* drawable = plugin->getIf<ml::UIComponent>()) {
     *     addComponent(*drawable);
     * }
     * @endcode
     *
     * ### Direct use
     * Most plugins should use @c PluginWith<Manifest> rather than inheriting
     * @c Plugin directly. @c PluginWith adds manifest support, static assertions
     * that enforce required manifest fields, and automatic thumbnail wiring.
     *
     * @see PluginWith, PluginManager, Messenger
     */
    class Plugin : public Messenger
    {
    public:
        virtual ~Plugin() = default;

        /**
         * @brief Return the plugin's display name.
         *
         * Overridden automatically by @c PluginWith to return
         * @c Manifest::name. Default returns @c "Unnamed Plugin".
         *
         * @return Null-terminated display name string.
         */
        virtual const char* getName()    const { return "Unnamed Plugin"; }

        /**
         * @brief Return the plugin's version string.
         *
         * Overridden automatically by @c PluginWith to return
         * @c Manifest::version. Default returns @c "1.0.0".
         *
         * @return Null-terminated version string (e.g., @c "1.0.0").
         */
        virtual const char* getVersion() const { return "1.0.0"; }

        /**
         * @brief Return the plugin's thumbnail texture, or @c nullptr.
         *
         * Overridden automatically by @c PluginWith when the manifest
         * declares @c Images::THUMBNAIL. Used by @c PluginManager::scanPlugins
         * to copy the texture before the probe dylib is closed.
         *
         * @return Pointer to the thumbnail @c sf::Texture, or @c nullptr.
         */
        virtual const sf::Texture* getThumbnail() const { return nullptr; }

        /**
         * @brief Called immediately after the plugin is constructed.
         *
         * Override to load textures, register components, or subscribe to
         * messages. The @c PluginManager and application infrastructure are
         * fully initialized before this is called.
         */
        virtual void onLoad()   {}

        /**
         * @brief Called immediately before the plugin is destroyed.
         *
         * Override to release resources, remove components, or perform any
         * cleanup that cannot happen in the destructor. After this returns,
         * the plugin object is deleted and its dylib is closed.
         */
        virtual void onUnload() {}

        /**
         * @brief Return a @c ml::Plugin* to this object.
         *
         * Provided so that @c ds::GamePlugin and other intermediate bases
         * can bridge back to @c ml::Plugin* for @c PluginManager without
         * requiring a @c dynamic_cast. @c PluginWith overrides this to
         * satisfy both @c ml::Plugin::asPlugin() and any pure-virtual
         * @c asPlugin() declared in a DaveStation interface.
         *
         * @return @c this, typed as @c ml::Plugin*.
         */
        virtual Plugin* asPlugin() { return this; }

        /**
         * @brief Return @c true if this plugin implements interface @c T.
         *
         * Uses @c dynamic_cast internally. Prefer @c getIf when you also
         * need the typed pointer.
         *
         * @tparam T Interface type to test for.
         * @return @c true if the cast succeeds.
         */
        template<typename T>
        bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

        /**
         * @brief Return a pointer to this plugin as interface @c T, or @c nullptr.
         *
         * Use this instead of a raw @c dynamic_cast when querying optional
         * plugin interfaces across dylib boundaries:
         * @code
         * if (auto* ui = plugin->getIf<ml::UIComponent>()) {
         *     addComponent(*ui);
         * }
         * @endcode
         *
         * @tparam T Interface type to cast to.
         * @return Typed pointer if the cast succeeds, @c nullptr otherwise.
         */
        template<typename T>
        T* getIf() { return dynamic_cast<T*>(this); }

        /** @brief Const overload of @c getIf(). */
        template<typename T>
        const T* getIf() const { return dynamic_cast<const T*>(this); }
    };

    // =========================================================================
    // TraitsHaveCore detection
    // =========================================================================

    /// @cond INTERNAL

    /**
     * @brief Detects whether any type in @c Traits inherits @c ml::Core.
     *
     * Used by @c PluginWith to select between @c PluginBase (no Core) and
     * @c PluginBaseWithCore (Core present via a trait such as a graphics component).
     */
    template<typename... Traits>
    struct TraitsHaveCore : std::disjunction<
        std::is_base_of<Core, Traits>...
    > {};

    // =========================================================================
    // PluginBase — no Core in traits
    // =========================================================================

    /**
     * @brief Internal base for plugins whose traits do not include @c ml::Core.
     *
     * Assembles @c Plugin, @c Customizable<Manifest>, the extra @c Traits, and
     * gathered flag/state stores into a single class. Enforces manifest validity
     * at compile time via @c static_assert.
     *
     * Do not inherit from this directly — use @c PluginWith<Manifest, Traits...>.
     */
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

    // =========================================================================
    // PluginBaseWithCore — Core present via traits
    // =========================================================================

    /**
     * @brief Internal base for plugins that include @c ml::Core through a trait.
     *
     * Identical to @c PluginBase but also exposes the system @c ml::Flag API
     * (@c FlagManager<ml::Flag>) alongside the custom flag API, resolving the
     * ambiguity that arises when @c Core's @c Flaggable and @c GatherFlags
     * both contribute @c enableFlag overloads.
     *
     * Do not inherit from this directly — use @c PluginWith<Manifest, Traits...>.
     */
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

        // System flags (from ml::Core → ml::Flaggable → FlagManager<ml::Flag>)
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // Custom flags from manifest + traits
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

    // =========================================================================
    // PluginWith — primary user-facing alias
    // =========================================================================

    /**
     * @brief Primary base class for manifest-driven plugins.
     *
     * @c PluginWith<Manifest, Traits...> is the type plugin authors inherit
     * from. It automatically selects @c PluginBase or @c PluginBaseWithCore
     * depending on whether any @c Trait introduces @c ml::Core into the
     * hierarchy (e.g., when the plugin is also a drawable component).
     *
     * ### Manifest requirements
     * The manifest must declare @c name and @c version as static constexpr
     * strings. The framework enforces this at compile time:
     * @code
     * class MyManifest : public ml::Manifest {
     * public:
     *     static constexpr const char* name    = "My Plugin";
     *     static constexpr const char* version = "1.0.0";
     *
     *     // Optional: enables automatic thumbnail support
     *     enum class Images { THUMBNAIL };
     *
     *     // Optional: custom flags and states
     *     enum class Flag  { Active };
     *     enum class State { Idle, Running };
     *
     * private:
     *     inline static const auto _ = [](){
     *         set(Images::THUMBNAIL, "assets/thumb.png");
     *         return 0;
     *     }();
     * };
     * @endcode
     *
     * ### Plugin implementation
     * @code
     * class MyPlugin : public ml::PluginWith<MyManifest>
     * {
     * public:
     *     void onLoad()   override { /* load resources *\/ }
     *     void onUnload() override { /* cleanup *\/ }
     * };
     * REGISTER_PLUGIN(MyPlugin)
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with @c name and @c version.
     * @tparam Traits   Optional extra traits (e.g., a graphics component base).
     *
     * @see Plugin, PluginManager, REGISTER_PLUGIN
     */
    template<typename Manifest, typename... Traits>
    using PluginWith = std::conditional_t<
        TraitsHaveCore<Traits...>::value,
        PluginBaseWithCore<Manifest, Traits...>,
        PluginBase<Manifest, Traits...>
    >;

} // namespace ml

// =============================================================================
// REGISTER_PLUGIN macro
// =============================================================================

/**
 * @defgroup PluginRegistration Plugin Registration
 * @{
 */

/**
 * @brief Export the factory and destructor functions required by @c PluginManager.
 *
 * Every plugin shared library must call this macro exactly once at file scope
 * in its @c .cpp file. It generates two @c extern @c "C" functions:
 * - @c createPlugin()  — constructs the plugin and returns @c ml::Plugin*
 * - @c destroyPlugin() — deletes the plugin
 *
 * ### Single-argument form (recommended for @c PluginWith plugins)
 * Name and version are read from the manifest automatically:
 * @code
 * // MyPlugin.cpp
 * #include "MyPlugin.h"
 * REGISTER_PLUGIN(MyPlugin)
 * @endcode
 *
 * ### Three-argument form (for plugins without @c PluginWith)
 * Supply name and version explicitly:
 * @code
 * REGISTER_PLUGIN(MyPlugin, "My Plugin", "2.0.0")
 * @endcode
 *
 * @param GameClass The concrete plugin class to instantiate.
 */
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

/// @cond INTERNAL
#define REGISTER_PLUGIN_PICK(_1, _2, _3, NAME, ...) NAME
/// @endcond

#define REGISTER_PLUGIN(...) \
    REGISTER_PLUGIN_PICK(__VA_ARGS__, REGISTER_PLUGIN_3, _UNUSED, REGISTER_PLUGIN_1)(__VA_ARGS__)

/** @} */
