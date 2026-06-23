// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Created by Dave Smith on 3/20/26.
//

#ifndef MALENA_CLICKABLE_H
#define MALENA_CLICKABLE_H

#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <optional>
namespace ml
{
    /**
     * @brief Trait that adds mouse-click callbacks to any @c Core object.
     * @ingroup Traits
     *
     * @c Clickable is automatically inherited by every @c ml::Core object.
     * A click fires when the mouse button is pressed and released over the
     * same component without leaving its bounds.
     *
     * ### Usage
     * @code
     * // No-argument callback
     * myRect.onClick([]{ std::cout << "clicked!\n"; });
     *
     * // Callback with raw SFML event data
     * myRect.onClick([](const std::optional<sf::Event>& e){
     *     if (e && e->is<sf::Event::MouseButtonReleased>())
     *         std::cout << "button: " << e->getIf<sf::Event::MouseButtonReleased>()->button << "\n";
     * });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myRect.unsubscribe(ml::Event::CLICK);
     * @endcode
     *
     * @see Hoverable, Focusable, ml::Event::CLICK, Unsubscribable
     */
    class Core;
    class MALENA_API Clickable : public EventReceiver
    {
    public:
        /**
         * @brief Register a no-argument callback invoked when this component is clicked.
         *
         * Replaces any previously registered @c onClick callback. The callback
         * fires after the mouse button is released over this component.
         *
         * @param callback Function invoked with no arguments on click.
         * @param overwrite When @c true, replaces any existing callback; when @c false, appends an additional one.
         */
        void onClick(std::function<void()> callback, bool overwrite = true);

        /**
         * @brief Register a callback invoked when this component is clicked,
         *        receiving the raw SFML event.
         *
         * Replaces any previously registered @c onClick callback unless
         * @p overwrite is @c false, in which case the callback is appended.
         *
         * @param callback Function invoked with the SFML event on click.
         * @param overwrite If @c true (default), replaces any existing callback.
         */
        void onClick(std::function<void(const std::optional<sf::Event>& event)> callback, bool overwrite = true);
    };

    /// @cond INTERNAL
    class MALENA_API ClickableDispatcher : public EventDispatcher
    {
        inline static EventReceiver* _focused = nullptr;

        // Topmost-wins: the single front-most component under the cursor that
        // passes the click gate this fire(). Only it receives the click, so a
        // click can't fall through to widgets painted behind it. Recomputed at
        // the start of each fire().
        Core* _topTarget = nullptr;

        // The per-component gate (positionable, effectively-visible, enabled,
        // under the exclusive owner, hovered) — shared by fire()'s topmost
        // search and filter().
        static bool passesClickGate(Core* component);

    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };
    /// @endcond

} // namespace ml

ML_EXPORT(ClickableDispatcher);
#endif // MALENA_CLICKABLE_H
