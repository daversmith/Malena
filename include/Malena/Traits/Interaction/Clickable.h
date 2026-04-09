// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/20/26.
//

#ifndef CLICKABLE_H
#define CLICKABLE_H

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
         */
        void onClick(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when this component is clicked,
         *        receiving the raw SFML event.
         *
         * Replaces any previously registered @c onClick callback.
         *
         * @param callback Function invoked with the SFML event on click.
         */
        void onClick(std::function<void(const std::optional<sf::Event>& event)> callback);
    };

    /// @cond INTERNAL
    class MALENA_API ClickableDispatcher : public EventDispatcher
    {
        inline static EventReceiver* _focused = nullptr;
    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };
    /// @endcond

} // namespace ml

ML_EXPORT(ClickableDispatcher);
#endif // CLICKABLE_H
