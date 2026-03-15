#ifndef COMPONENT_H
#define COMPONENT_H

#include <Malena/Core/Core.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Resources/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Manifests/Manifest.h>
#include "SFML/Graphics/Drawable.hpp"
#include <type_traits>

namespace ml
{
    /**
     * @brief Compile-time guard that rejects traits already provided by @c Core.
     *
     * @c Core already inherits @c Subscribable, @c Flaggable, and @c Positionable.
     * If any of those are passed as extra @c Traits to @c ComponentCore, this
     * predicate evaluates to @c true and the @c static_assert in @c ComponentCore
     * fires with a clear error message.
     *
     * @tparam Traits The extra trait types to check.
     */
    template<typename... Traits>
    struct HasCoreTraits : std::disjunction<
        std::is_same<Traits, Subscribable>...,
        std::is_same<Traits, Flaggable>...,
        std::is_same<Traits, Positionable>...
    > {};

    /**
     * @brief Internal non-drawable layer of the component hierarchy.
     *
     * @c ComponentCore assembles everything a Malena component needs except
     * @c sf::Drawable. It is used by two different paths:
     *
     * - @c ComponentBase (standard components) inherits @c ComponentCore and
     *   then adds @c sf::Drawable, giving a full drawable component.
     * - Graphics primitives (e.g., @c Graphic<sf::RectangleShape>) inherit
     *   @c ComponentCore directly and supply drawable behavior through their
     *   own SFML shape type, deliberately bypassing click/hover registration.
     *
     * ### What @c ComponentCore provides
     * - Everything in @c Core (@c Subscribable, @c Flaggable, @c Positionable)
     * - @c Draggable — drag handling gated by @c ml::Flag::DRAGGABLE
     * - Any extra @c Traits passed as template parameters
     * - @c GatherFlags — a @c MultiCustomFlaggable that collects the @c Flags
     *   enum from the manifest and from each trait that declares one
     * - @c GatherStates — a @c MultiCustomStateManager that does the same for
     *   @c State enums
     * - Unified @c using declarations that bring both system-flag (@c ml::Flag)
     *   and custom-flag APIs into the same overload set
     *
     * ### Drag subscription
     * The constructor subscribes to the @c "draggable" event. The callback
     * checks @c ml::Flag::DRAGGABLE at runtime, so it is a no-op until
     * dragging is explicitly enabled:
     * @code
     * myComponent.setFlag(ml::Flag::DRAGGABLE);
     * @endcode
     *
     * @tparam ComponentManifest Manifest struct declaring @c Flags/@c State enums
     *                           and resource lists. Defaults to @c void (no manifest).
     * @tparam Traits            Additional trait types to mix in (must not include
     *                           @c Subscribable, @c Flaggable, or @c Positionable).
     *
     * @see ComponentBase, Component, Draggable, GatherFlags, GatherStates
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
     * @brief Intermediate layer that adds @c sf::Drawable and event participation.
     *
     * @c ComponentBase inherits @c ComponentCore and @c sf::Drawable, then
     * registers empty @c onClick and @c onHover callbacks in its constructor.
     * These empty registrations ensure the component appears in
     * @c EventsManager's subscriber map, so it receives hit-test checks from
     * @c UIManager even before user code attaches meaningful handlers.
     *
     * @note Internal graphics primitives (e.g., @c ml::Rectangle's background
     *       shape) inherit @c ComponentCore directly and intentionally skip
     *       this step — they must @b not intercept click/hover events.
     *
     * @tparam ComponentManifest Manifest struct, forwarded to @c ComponentCore.
     * @tparam Traits            Extra traits, forwarded to @c ComponentCore.
     *
     * @see ComponentCore, Component
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
     * @brief Primary base class for all user-facing Malena components.
     *
     * @c Component is the type most framework users and students will inherit
     * from when creating custom components. It dispatches to @c ComponentBase
     * based on whether the first template argument is a @c Manifest subclass:
     *
     * - If @c First derives from @c Manifest, it is treated as the manifest
     *   and forwarded as @c ComponentManifest.
     * - Otherwise, @c First is treated as the first extra trait and
     *   @c ComponentManifest defaults to @c void.
     *
     * This lets both forms work without user disambiguation:
     * @code
     * class HUD      : public ml::Component<> {};                      // no manifest
     * class Carousel : public ml::Component<CarouselManifest> {};      // with manifest
     * class Widget   : public ml::Component<WidgetManifest, Draggable> {}; // manifest + trait
     * @endcode
     *
     * The convenience alias @c ml::ComponentWith<M, Traits...> is equivalent
     * to @c ml::Component<M, Traits...> and may read more clearly at the
     * call site.
     *
     * @tparam First  Either a @c Manifest subclass or the first extra trait.
     *                Defaults to @c void (no manifest, no extra traits).
     * @tparam Rest   Additional traits (only meaningful when @c First is a manifest).
     *
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
     *
     * Provided as a more expressive alternative when the intent is clearly
     * "component with this manifest":
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
