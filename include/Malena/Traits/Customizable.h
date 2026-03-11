#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <SFML/Graphics.hpp>
#include <Malena/Traits/CustomFlaggable.h>
#include <Malena/Managers/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Managers/StateManager.h>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    /**
     * @brief Adds manifest-driven custom flags and state to a Component.
     *
     * Takes Component as first template parameter and inherits it directly.
     * Uses FlagManager<ml::Flag> (not Component) for system flag using
     * declarations — this is always one unique class in the hierarchy
     * regardless of how many traits are added, so it is never ambiguous.
     *
     * Traits may freely inherit CustomFlaggable<TheirManifest::Flags>
     * without causing ambiguity.
     *
     * @code
     * // No traits
     * class Carousel : public ml::ComponentWith<CarouselManifest> {};
     *
     * // With traits
     * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
     *
     * // Standalone (no Component base)
     * class FireableBase : public ml::Customizable<FireableBaseManifest> {};
     *
     * // Trait with manifest
     * class Draggable : public ml::Customizable<DraggableManifest> {};
     * @endcode
     *
     * @tparam Manifest  A class inheriting ml::Manifest
     * @tparam Component The component base, or void for standalone use
     */
    template<typename Manifest, typename Component = void>
    class Customizable : public Component,
                         public CustomFlaggable<typename extract_Flags<Manifest>::type>,
                         public StateManager<typename extract_State<Manifest>::type>
    {
    private:
        using FlagType  = typename extract_Flags<Manifest>::type;
        using StateType = typename extract_State<Manifest>::type;

    public:
        Customizable() = default;
        virtual ~Customizable() = default;

        // Forwarding constructor
        template<typename Arg, typename... Args,
                 typename = std::enable_if_t<!std::is_same_v<std::decay_t<Arg>, Customizable>>>
        explicit Customizable(Arg&& arg, Args&&... args);

        // ── System flag passthrough — FlagManager<ml::Flag> is always unique ──
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // ── Custom manifest flag overloads ────────────────────────────────────
        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void enableFlag(F flag);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void disableFlag(F flag);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        bool checkFlag(F flag) const;

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void setFlag(F flag, bool value);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void toggleFlag(F flag);
    };

    // ── Standalone specialization — no Component base ─────────────────────────
    template<typename Manifest>
    class Customizable<Manifest, void> : public virtual FlagManager<ml::Flag>,
                                         public CustomFlaggable<typename extract_Flags<Manifest>::type>,
                                         public StateManager<typename extract_State<Manifest>::type>
    {
    private:
        using FlagType  = typename extract_Flags<Manifest>::type;
        using StateType = typename extract_State<Manifest>::type;

    public:
        Customizable() = default;
        virtual ~Customizable() = default;

        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void enableFlag(F flag);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void disableFlag(F flag);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        bool checkFlag(F flag) const;

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void setFlag(F flag, bool value);

        template<typename F = FlagType, typename = std::enable_if_t<!std::is_void_v<F>>>
        void toggleFlag(F flag);
    };

} // namespace ml

#include "../../../src/Traits/Customizable.cpp"
#endif // UICOMPONENT_H