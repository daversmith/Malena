#ifndef COMPONENT_H
#define COMPONENT_H

#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/ManifestResources.h>
#include <Malena/Resources/FlagManager.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Utilities/Flag.h>
#include <type_traits>
#include "SFML/Graphics/Drawable.hpp"

#include "Malena/Manifests/ManifestResources.h"

namespace ml
{
    /// @cond INTERNAL
    template<typename... Traits>
    struct HasCoreTraits : std::disjunction<
        std::is_same<Traits, Subscribable>...,
        std::is_same<Traits, Flaggable>...,
        std::is_same<Traits, Positionable>...
    > {};
    /// @endcond

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
            "Subscribable, Flaggable, and Positionable are already included via Core."
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

    /**
     * @brief Intermediate drawable layer — with manifest.
     * @ingroup Core
     *
     * When a manifest is provided, injects:
     * - @c ManifestAliases — enum types unqualified in scope via @c Resources
     * - @c Resources — unified accessor: @c Resources::get(Images::Background),
     *   @c Resources::get(Text::Title) etc.
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
        using Resources = ml::ManifestResources<ComponentManifest>;
    };

    /// @cond INTERNAL
    template<typename... Traits>
    struct ComponentBase<void, Traits...> : public sf::Drawable,
                                            public ComponentCore<void, Traits...>
    {
    };
    /// @endcond

    /**
     * @brief Primary base class for all user-facing Malena components.
     * @ingroup Core
     *
     * @code
     * // No manifest
     * class HUD : public ml::Component<> {};
     *
     * // With manifest — gets Resources::get() automatically
     * class MyWidget : public ml::ComponentWith<MyManifest>
     * {
     *     void initialization() override
     *     {
     *         auto& tex   = Resources::get(Images::Background);
     *         auto& title = Resources::get(Text::WindowTitle);
     *     }
     * };
     * @endcode
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

    /**
     * @brief Alias for @c Component<M, Traits...>.
     * @ingroup Core
     *
     * @code
     * class MyWidget : public ml::ComponentWith<MyManifest> {};
     * @endcode
     *
     * @tparam M      The manifest type.
     * @tparam Traits Optional additional traits.
     */
    template<typename M, typename... Traits>
    using ComponentWith = Component<M, Traits...>;

} // namespace ml

#endif // COMPONENT_H
