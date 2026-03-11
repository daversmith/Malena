//
// Created by Dave Smith on 3/8/26.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include <Malena/Interfaces/Core.h>
#include <Malena/Traits/MultiCustomFlaggable.h>
#include <Malena/Managers/StateManager.h>
#include <Malena/Managers/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Utilities/TypeExtraction.h>
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
     * @brief Internal base — Core + sf::Drawable + traits + flags + state.
     *
     * Gathers all flag enums from ComponentManifest and each trait's
     * manifest_type into one MultiCustomFlaggable. Adds StateManager
     * for the component manifest's State enum.
     *
     * @tparam ComponentManifest The component's own manifest (or void)
     * @tparam Traits            Optional opt-in traits
     */
    template<typename ComponentManifest, typename... Traits>
    struct ComponentBase : public sf::Drawable,
                           public Core,
                           public Draggable,
                           public Traits...,
                           public GatherFlags<ComponentManifest, Draggable, Traits...>::type,
                           public StateManager<typename extract_State<ComponentManifest>::type>
    {
        static_assert(
            !HasCoreTraits<Traits...>::value,
            "Subscribable, Flaggable, and Positionable are already included in Component via Core. "
            "Remove them from the trait list."
        );

        ComponentBase() {
            EventsManager::subscribe("draggable", this, [this](const std::optional<sf::Event>& event)
            {
                if (checkFlag(ml::Flag::DRAGGABLE))
                    Draggable::handleDragEvent(event);
            });
        }

        // System flags — FlagManager<ml::Flag> is always unique in hierarchy
        using FlagManager<ml::Flag>::enableFlag;
        using FlagManager<ml::Flag>::disableFlag;
        using FlagManager<ml::Flag>::checkFlag;
        using FlagManager<ml::Flag>::setFlag;
        using FlagManager<ml::Flag>::toggleFlag;

        // Custom flags — restore visibility hidden by system flag using declarations
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::enableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::disableFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::checkFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::setFlag;
        using GatherFlags<ComponentManifest, Draggable, Traits...>::type::toggleFlag;
    };

    /**
     * @brief Plain component — no manifest, optional traits.
     * @code
     * class HUD   : public ml::Component<> {};
     * class Panel : public ml::Component<Draggable> {};
     * @endcode
     */
    template<typename First = void, typename... Rest>
    class Component : public ComponentBase<void, First, Rest...> {};

    template<>
    class Component<void> : public ComponentBase<void> {};

    /**
     * @brief Component with manifest and optional traits.
     * @code
     * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
     * @endcode
     */
    template<typename Manifest, typename... Traits>
    class ComponentWith : public ComponentBase<Manifest, Traits...> {};

} // namespace ml

#endif //COMPONENT_H