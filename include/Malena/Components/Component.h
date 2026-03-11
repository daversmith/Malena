//
// Created by Dave Smith on 3/8/26.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include <Malena/Interfaces/Core.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>
#include "SFML/Graphics/Drawable.hpp"
#include <type_traits>

namespace ml
{
    /**
     * @brief Checks if any of Traits is already provided by Core.
     * Used to give a clear compile error if user adds redundant traits.
     */
    template<typename... Traits>
    struct HasCoreTraits : std::disjunction<
        std::is_same<Traits, Subscribable>...,
        std::is_same<Traits, Flaggable>...,
        std::is_same<Traits, Positionable>...
    > {};

    /**
     * @brief Internal base — Core + sf::Drawable + opt-in traits.
     * @tparam Traits Optional traits e.g. Draggable, Messenger
     */
    template<typename... Traits>
    struct ComponentBase : public sf::Drawable, public Core, public Traits...
    {
        static_assert(
            !HasCoreTraits<Traits...>::value,
            "Subscribable, Flaggable, and Positionable are already included in Component via Core. "
            "Remove them from the trait list."
        );
    };

    /**
     * @brief Base component class for all Malena components.
     *
     * Use Component<> for a plain component with no traits and no manifest.
     * Use Component<Trait1, Trait2> to opt into additional traits.
     *
     * To add a manifest, inherit Customizable<MyManifest> alongside:
     * @code
     * class Carousel : public ml::Component<Draggable>,
     *                  public ml::Customizable<CarouselManifest> {};
     * @endcode
     *
     * @tparam Traits Optional opt-in traits (e.g. Draggable, Messenger)
     */
    template<typename First = void, typename... Rest>
    class Component : public ComponentBase<First, Rest...> {};

    /**
     * @brief Specialization for no traits — plain component with Core only.
     */
    template<>
    class Component<void> : public ComponentBase<> {};

} // namespace ml

#endif //COMPONENT_H