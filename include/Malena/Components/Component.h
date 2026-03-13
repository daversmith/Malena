#ifndef COMPONENT_H
#define COMPONENT_H

#include <Malena/Interfaces/Core.h>
#include <Malena/Traits/MultiCustomFlaggable.h>
#include <Malena/Traits/MultiCustomStateManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Managers/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Manifests/Manifest.h>
#include "SFML/Graphics/Drawable.hpp"
#include <type_traits>

namespace ml
{
    template<typename... Traits>
    struct HasCoreTraits : std::disjunction<
        std::is_same<Traits, Subscribable>...,
        std::is_same<Traits, Flaggable>...,
        std::is_same<Traits, Positionable>...
    > {};

    /**
     * @brief Internal core — everything except sf::Drawable.
     *
     * Used by both ComponentBase (standard components) and Graphic
     * (which provides its own sf::Drawable via the shape type T).
     *
     * Does NOT register onClick/onHover — Graphic members should not
     * participate in click/hover detection unless explicitly subscribed.
     * Only the draggable subscription is registered here, gated by
     * ml::Flag::DRAGGABLE so it's a no-op unless dragging is enabled.
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
            EventsManager::subscribe("draggable", this, [this](const std::optional<sf::Event>& event)
            {
                if (checkFlag(ml::Flag::DRAGGABLE))
                    Draggable::handleDragEvent(event);
            });
        }

        // System flags
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // Custom flags
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::enableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::disableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::checkFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::setFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::toggleFlag;

        // Custom states
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::setState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::getState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::isState;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::onStateEnter;
        using GatherStates<ComponentManifest, Draggable, Traits...>::type::onStateExit;
    };

    /**
     * @brief Internal component base — adds sf::Drawable to ComponentCore.
     * Registers empty onClick/onHover so this component participates in
     * EventsManager iteration. Internal Graphic members inherit ComponentCore
     * directly and intentionally skip this — they must not intercept events.
     */
    template<typename ComponentManifest, typename... Traits>
    struct ComponentBase : public sf::Drawable,
                           public ComponentCore<ComponentManifest, Traits...>
    {
        ComponentBase()
        {
            this->onClick([](){});
            this->onHover([](){});
        }
    };

    /**
     * @brief Base class for all Malena components.
     *
     * @code
     * class HUD      : public ml::Component<> {};
     * class Carousel : public ml::Component<CarouselManifest> {};
     * @endcode
     */
    template<typename First = void, typename... Rest>
    class Component : public std::conditional_t<
        std::is_base_of_v<Manifest, First>,
        ComponentBase<First, Rest...>,
        ComponentBase<void, First, Rest...>
    > {};

    template<>
    class Component<void> : public ComponentBase<void> {};

    template<typename M, typename... Traits>
    using ComponentWith = Component<M, Traits...>;

} // namespace ml

#endif //COMPONENT_H