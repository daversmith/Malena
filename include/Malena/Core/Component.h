
// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_COMPONENT_H
#define MALENA_COMPONENT_H

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Interaction/Draggable.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/ManifestResources.h>
#include <Malena/Resources/AssetsManager.h>
#include <Malena/Resources/FlagManager.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Utilities/Flag.h>
#include <type_traits>
#include <SFML/Graphics/Drawable.hpp>

namespace ml
{
    // =========================================================================
    // Manifest collision detection helpers
    // =========================================================================

    /// @cond INTERNAL

    /**
     * @brief Counts how many types in a pack declare an @c Images enum.
     * Used to detect manifest collisions in @c ComponentBase.
     */
    template<typename... Ts>
    struct ImageCount
    {
        static constexpr int value = (has_Image<Ts>::value + ... + 0);
    };

    /**
     * @brief Counts how many types in a pack declare a @c Fonts enum.
     */
    template<typename... Ts>
    struct FontCount
    {
        static constexpr int value = (has_Font<Ts>::value + ... + 0);
    };

    /**
     * @brief Counts how many types in a pack declare a @c Sounds enum.
     */
    template<typename... Ts>
    struct SoundCount
    {
        static constexpr int value = (has_Sound<Ts>::value + ... + 0);
    };

    /**
     * @brief Counts how many types in a pack declare a @c Text enum.
     */
    template<typename T, typename = void>
    struct has_Text : std::false_type {};
    template<typename T>
    struct has_Text<T, std::void_t<typename T::Text>> : std::true_type {};

    template<typename... Ts>
    struct TextCount
    {
        static constexpr int value = (has_Text<Ts>::value + ... + 0);
    };

    /**
     * @brief Extracts the manifest type from a trait if it has one,
     *        otherwise yields void.
     */
    template<typename T, typename = void>
    struct extract_manifest_or_void { using type = void; };

    template<typename T>
    struct extract_manifest_or_void<T, std::void_t<typename T::manifest_type>>
    { using type = typename T::manifest_type; };

    /// @endcond

    // =========================================================================
    // HasCoreTraits
    // =========================================================================

    /// @cond INTERNAL
    template<typename... Traits>
    struct HasCoreTraits : std::disjunction<
        std::is_same<Traits, Subscribable>...,
        std::is_same<Traits, Flaggable>...,
        std::is_same<Traits, Positionable>...
    > {};
    /// @endcond

    // =========================================================================
    // ComponentCore
    // =========================================================================

    /**
     * @brief Internal non-drawable layer of the component hierarchy.
     * @ingroup Core
     * @see Component, Draggable, GatherFlags, GatherStates
     */
    template<typename ComponentManifest = void, typename... Traits>
    struct ComponentCore : public Core,
                           public Draggable,
                           public Traits...,
                           public GatherFlags <ComponentManifest, Draggable, Traits...>::type,
                           public GatherStates<ComponentManifest, Draggable, Traits...>::type
    {
        static_assert(
            !HasCoreTraits<Traits...>::value,
            "[Malena] Subscribable, Flaggable, and Positionable are already "
            "included via Core — do not pass them as traits."
        );

        ComponentCore()
        {
            EventManager::subscribe(ml::Event::DRAG, static_cast<Draggable*>(this));
            this->onClick([](){});
            this->onHover([](){});
        }

        // ── System flags (ml::Flag) ──────────────────────────────────────────
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // ── Custom flags from manifest + traits ──────────────────────────────
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::enableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::disableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::checkFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::setFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::toggleFlag;

        // ── Custom states from manifest + traits ─────────────────────────────
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::setState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::getState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::isState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::onStateEnter;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::onStateExit;
    };

    // =========================================================================
    // ComponentBase — with manifest
    // =========================================================================

    /**
     * @brief Intermediate drawable layer — with manifest.
     * @ingroup Core
     *
     * Injects @c ManifestAliases and @c Resources into scope when a manifest
     * is provided. Also performs compile-time checks to detect manifest
     * collisions — if two manifests in the inheritance chain both declare
     * @c Images, @c Fonts, @c Sounds, or @c Text, a clear error is emitted
     * pointing the user to use fully qualified syntax.
     *
     * @tparam ComponentManifest Manifest struct forwarded to @c ComponentCore.
     * @tparam Traits            Extra traits forwarded to @c ComponentCore.
     * @see ComponentCore, Component, ManifestResources
     */
    template<typename ComponentManifest, typename... Traits>
    struct ComponentBase : public sf::Drawable,
                           public ComponentCore<ComponentManifest, Traits...>,
                           public ManifestResources<ComponentManifest>
    {
    private:
        // Collect all manifest types from traits for collision checking
        // (component manifest + each trait's manifest_type if it has one)
        using _AllManifests = std::tuple<
            ComponentManifest,
            typename extract_manifest_or_void<Traits>::type...
        >;

        // ── Images collision check ────────────────────────────────────────────
        static_assert(
            ImageCount<
                ComponentManifest,
                typename extract_manifest_or_void<Traits>::type...
            >::value <= 1,
            "[Malena] Images alias is ambiguous — two or more manifests in "
            "this component's inheritance chain both declare an Images enum. "
            "Use MyManifest::Images::VALUE syntax to qualify explicitly."
        );

        // ── Fonts collision check ─────────────────────────────────────────────
        static_assert(
            FontCount<
                ComponentManifest,
                typename extract_manifest_or_void<Traits>::type...
            >::value <= 1,
            "[Malena] Fonts alias is ambiguous — two or more manifests in "
            "this component's inheritance chain both declare a Fonts enum. "
            "Use MyManifest::Fonts::VALUE syntax to qualify explicitly."
        );

        // ── Sounds collision check ────────────────────────────────────────────
        static_assert(
            SoundCount<
                ComponentManifest,
                typename extract_manifest_or_void<Traits>::type...
            >::value <= 1,
            "[Malena] Sounds alias is ambiguous — two or more manifests in "
            "this component's inheritance chain both declare a Sounds enum. "
            "Use MyManifest::Sounds::VALUE syntax to qualify explicitly."
        );

        // ── Text collision check ──────────────────────────────────────────────
        static_assert(
            TextCount<
                ComponentManifest,
                typename extract_manifest_or_void<Traits>::type...
            >::value <= 1,
            "[Malena] Text alias is ambiguous — two or more manifests in "
            "this component's inheritance chain both declare a Text enum. "
            "Use MyManifest::Text::VALUE syntax to qualify explicitly."
        );

    public:
        using Resources = ml::ManifestResources<ComponentManifest>;
    };

    // =========================================================================
    // ComponentBase — no manifest
    // =========================================================================

    /// @cond INTERNAL
    template<typename... Traits>
    struct ComponentBase<void, Traits...> : public sf::Drawable,
                                            public ComponentCore<void, Traits...>
    {
    };
    /// @endcond

    // =========================================================================
    // Component
    // =========================================================================

    /**
     * @brief Primary base class for all user-facing Malena components.
     * @ingroup Core
     *
     * @code
     * // No manifest
     * class HUD : public ml::Component<> {};
     *
     * // With manifest — gets Resources::get() and alias injection automatically
     * class MyWidget : public ml::ComponentWith<MyManifest>
     * {
     *     MyWidget()
     *     {
     *         auto& tex   = Resources::get(Images::Background);
     *         auto& title = Resources::get(Text::WindowTitle);
     *     }
     * };
     * @endcode
     *
     * If two manifests in the inheritance chain declare the same resource enum
     * (@c Images, @c Fonts, @c Sounds, @c Text), a compile-time error is emitted
     * with a clear message pointing to the resolution.
     *
     * @tparam First  Either a @c Manifest subclass or the first extra trait.
     * @tparam Rest   Additional traits.
     * @see ComponentBase, ComponentCore, ComponentWith
     */
    template<typename First = void, typename... Rest>
    class Component : public std::conditional_t<
        std::is_base_of_v<Manifest, First>,
        ComponentBase<First, Rest...>,
        ComponentBase<void, First, Rest...>
    > {};

    /// @cond INTERNAL
    template<>
    class Component<void> : public ComponentBase<void> {};
    /// @endcond

    // =========================================================================
    // ComponentWith alias
    // =========================================================================

    /**
     * @brief Alias for @c Component<M, Traits...>.
     * @ingroup Core
     *
     * @code
     * class MyWidget : public ml::ComponentWith<MyManifest> {};
     * class MyWidget : public ml::ComponentWith<MyManifest, Draggable> {};
     * @endcode
     *
     * @tparam M      The manifest type.
     * @tparam Traits Optional additional traits.
     */
    template<typename M, typename... Traits>
    using ComponentWith = Component<M, Traits...>;

} // namespace ml

#endif // MALENA_COMPONENT_H