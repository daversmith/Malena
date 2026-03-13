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
#include <Malena/Utilities/Flag.h>
#include <Malena/Interfaces/Core.h>
#include <type_traits>

namespace ml {

    class Plugin : public Messenger
    {
    public:
        virtual ~Plugin() = default;
        virtual const char* getName()    const { return "Unnamed Plugin"; }
        virtual const char* getVersion() const { return "1.0"; }
        virtual void onLoad()   {}
        virtual void onUnload() {}
        virtual Plugin* asPlugin() { return this; }  // safe upcast through vtable

        template<typename T>
        bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

        template<typename T>
        T* getIf() { return dynamic_cast<T*>(this); }

        template<typename T>
        const T* getIf() const { return dynamic_cast<const T*>(this); }
    };

    // Detect whether any trait brings Core (and thus Flaggable) into the hierarchy
    template<typename... Traits>
    struct TraitsHaveCore : std::disjunction<
        std::is_base_of<Core, Traits>...
    > {};

    // ── PluginBase — custom flags+states only (no Core in traits) ─────────────
    template<typename Manifest, typename... Traits>
    struct PluginBase : public Plugin,
                        public Customizable<Manifest>,
                        public Traits...,
                        public GatherFlags <Manifest, Traits...>::type,
                        public GatherStates<Manifest, Traits...>::type
    {
        using manifest_type = Manifest;

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

    // ── PluginBaseWithCore — adds system flag usings when Core is present ─────
    // Used when a trait like ds::GraphicInterface brings Core into the hierarchy,
    // which also brings Flaggable (FlagManager<ml::Flag>). Both overload sets
    // are collapsed here so the user never has to write using declarations.
    template<typename Manifest, typename... Traits>
    struct PluginBaseWithCore : public Plugin,
                                public Customizable<Manifest>,
                                public Traits...,
                                public GatherFlags <Manifest, Traits...>::type,
                                public GatherStates<Manifest, Traits...>::type
    {
        using manifest_type = Manifest;

        // System flags — available because Core is in Traits
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
     * @brief Plugin with manifest support — flags and states gathered automatically.
     *
     * Pass ds::GraphicInterface (or any Core-derived trait) to get visual support:
     * @code
     * // Non-visual plugin
     * class AudioEngine : public ml::PluginWith<AudioManifest> {};
     *
     * // Visual plugin — can be added to the scene
     * class MainScreen : public ml::PluginWith<MainScreenManifest, ds::GraphicInterface> {};
     * @endcode
     */
    template<typename Manifest, typename... Traits>
    using PluginWith = std::conditional_t<
        TraitsHaveCore<Traits...>::value,
        PluginBaseWithCore<Manifest, Traits...>,
        PluginBase<Manifest, Traits...>
    >;

} // namespace ml

#define REGISTER_PLUGIN(GameClass) \
extern "C" { \
PLUGIN_EXPORT ml::Plugin* createPlugin() { \
    return (new GameClass())->asPlugin(); \
} \
PLUGIN_EXPORT void destroyPlugin(ml::Plugin* plugin) { \
    delete plugin; \
} \
}