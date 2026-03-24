// Plugin.h
#pragma once

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT
#endif

#include <Malena/Traits/Base/Customizable.h>
#include <Malena/Core/Core.h>
#include <Malena/Core/Export.h> // Fireable-only ML_EXPORT
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
    // =========================================================================
    // Manifest validation traits
    // =========================================================================

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

    // =========================================================================
    // Plugin — virtual base class
    // =========================================================================

    /**
     * @brief Abstract base class for all Malena plugins.
     * @ingroup EnginePlugins
     * @see PluginWith, PluginManager, ML_EXPORT
     */
    class Plugin : public Messenger
    {
    public:
        virtual ~Plugin() = default;

        virtual const char*        getName()      const { return "Unnamed Plugin"; }
        virtual const char*        getVersion()   const { return "1.0.0"; }
        virtual const sf::Texture* getThumbnail() const { return nullptr; }
        virtual void               onLoad()             {}
        virtual void               onUnload()           {}
        virtual Plugin*            asPlugin()           { return this; }

        template<typename T>
        bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

        template<typename T>
        T* getIf() { return dynamic_cast<T*>(this); }

        template<typename T>
        const T* getIf() const { return dynamic_cast<const T*>(this); }
    };

    // =========================================================================
    // TraitsHaveCore
    // =========================================================================

    /// @cond INTERNAL

    template<typename... Traits>
    struct TraitsHaveCore : std::disjunction<
        std::is_base_of<Core, Traits>...
    > {};

    // =========================================================================
    // PluginBase
    // =========================================================================

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
    // PluginBaseWithCore
    // =========================================================================

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

    // =========================================================================
    // PluginWith
    // =========================================================================

    /**
     * @brief Primary base class for manifest-driven plugins.
     *
     * @code
     * class MyPlugin : public ml::PluginWith<MyManifest>
     * {
     * public:
     *     void onLoad()   override { }
     *     void onUnload() override { }
     * };
     * ML_EXPORT(MyPlugin)
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with @c name and @c version.
     * @tparam Traits   Optional extra traits.
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
//  FireableHelper — full specialization
//
//  Now that ml::Plugin is complete, we can correctly evaluate
//  !std::is_base_of_v<ml::Plugin, T>. This specialization replaces
//  the no-op primary template from Fireable.h for Fireable-only types.
//
//  Plugin-only types   → PluginHelper handles them, FireableHelper is no-op
//  Fireable-only types → this specialization registers the singleton
//  Plugin+Fireable     → excluded here, PluginManager registers at loadPlugin
// =============================================================================



// =============================================================================
//  PluginHelper
//
//  Same pattern — new T() only compiled for actual Plugin types.
// =============================================================================

namespace ml::exports
{
    /**
     * @brief Primary template — no-op for non-Plugin types.
     * @cond INTERNAL
     */
    template<typename T, typename = void>
    struct PluginHelper
    {
        static ml::Plugin* create()             { return nullptr; }
        static void        destroy(ml::Plugin*) {}
    };

    /**
     * @brief Specialisation for Plugin types — constructs and destroys.
     * @cond INTERNAL
     */
    template<typename T>
    struct PluginHelper<T, std::enable_if_t<std::is_base_of_v<ml::Plugin, T>>>
    {
        static ml::Plugin* create()               { return (new T())->asPlugin(); }
        static void        destroy(ml::Plugin* p) { delete p; }
    };
    /** @endcond */
}

// =============================================================================
//  ML_EXPORT — full version
//  Plugin and Fireable both complete — all helpers work correctly.
// =============================================================================

#undef ML_EXPORT

/**
 * @def ML_EXPORT(ClassName)
 * @brief Register any exportable Malena type with the framework.
 *
 * - **Plugin** — emits @c extern @c "C" factory symbols via @c PluginHelper.
 * - **Fireable** — registers singleton via @c FireableHelper.
 * - **Plugin+Fireable** — Plugin path only; @c PluginManager registers
 *   the Fireable aspect at @c loadPlugin time.
 * - **Neither** — @c static_assert compile error.
 *
 * @param ClassName The class to register. Must be fully defined.
 */
#define ML_EXPORT(ClassName)                                                        \
                                                                                    \
    /* ── Plugin path ────────────────────────────────────────────────────────── */ \
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
    /* ── Fireable path ──────────────────────────────────────────────────────── */ \
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
