// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/20/26.
//

#ifndef HOVERABLE_H
#define HOVERABLE_H

#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <optional>
namespace ml
{
    /**
     * @brief Trait that adds mouse-hover and mouse-leave callbacks to any @c Core object.
     * @ingroup Traits
     *
     * @c Hoverable is automatically inherited by every @c ml::Core object.
     * The hover event fires once when the mouse cursor enters a component's
     * bounds; the unhover event fires once when it leaves.
     *
     * ### Usage
     * @code
     * // Highlight on hover, restore on leave
     * myRect.onHover([&]{ myRect.setFillColor(sf::Color::Yellow); });
     * myRect.onUnhover([&]{ myRect.setFillColor(sf::Color::White); });
     *
     * // With SFML event data
     * myRect.onHover([](const std::optional<sf::Event>& e){
     *     // e holds the MouseMoved event that triggered the enter
     * });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myRect.unsubscribe(ml::Event::HOVER);
     * myRect.unsubscribe(ml::Event::UNHOVER);
     * @endcode
     *
     * @see Clickable, Focusable, ml::Event::HOVER, ml::Event::UNHOVER, Unsubscribable
     */
    class MALENA_API Hoverable : public EventReceiver
    {
    public:
        /**
         * @brief Register a no-argument callback invoked when the mouse enters
         *        this component's bounds.
         *
         * @param callback Function invoked with no arguments on hover enter.
         */
        void onHover(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the mouse enters this
         *        component's bounds, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on hover enter.
         */
        void onHover(std::function<void(const std::optional<sf::Event>& event)> callback);

        /**
         * @brief Register a no-argument callback invoked when the mouse leaves
         *        this component's bounds.
         *
         * @param callback Function invoked with no arguments on hover leave.
         */
        void onUnhover(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the mouse leaves this
         *        component's bounds, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on hover leave.
         */
        void onUnhover(std::function<void(const std::optional<sf::Event>& event)> callback);
    };

    /// @cond INTERNAL
    class MALENA_API HoverableDispatcher : public EventDispatcher
    {
    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };
    /// @endcond

} // namespace ml

ML_EXPORT(HoverableDispatcher)
#endif // HOVERABLE_H
