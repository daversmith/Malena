// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/21/26.
//

#ifndef FOCUSABLE_H
#define FOCUSABLE_H

#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <optional>
namespace ml
{
    /**
     * @brief Trait that adds keyboard-focus and blur callbacks to any @c Core object.
     * @ingroup Traits
     *
     * @c Focusable is automatically inherited by every @c ml::Core object.
     * A component gains focus when the user clicks it; it loses focus (blurs)
     * when the user clicks elsewhere or focus is programmatically transferred.
     * While focused, a component receives keyboard events via @c Keyable.
     *
     * ### Usage
     * @code
     * // Show a focus ring on focus, hide it on blur
     * myInput.onFocus([&]{ focusRing.setOutlineThickness(2.f); });
     * myInput.onBlur([&]{ focusRing.setOutlineThickness(0.f); });
     *
     * // With SFML event data
     * myInput.onFocus([](const std::optional<sf::Event>& e){
     *     // e holds the MouseButtonReleased event that triggered focus
     * });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myInput.unsubscribe(ml::Event::FOCUS);
     * myInput.unsubscribe(ml::Event::BLUR);
     * @endcode
     *
     * @see Keyable, Clickable, ml::Event::FOCUS, ml::Event::BLUR, Unsubscribable
     */
    class MALENA_API Focusable : public EventReceiver
    {
    public:
        /**
         * @brief Register a no-argument callback invoked when this component
         *        gains keyboard focus.
         *
         * @param callback Function invoked with no arguments when focus is gained.
         */
        void onFocus(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when this component gains keyboard
         *        focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event when focus is gained.
         */
        void onFocus(std::function<void(const std::optional<sf::Event>&)> callback);

        /**
         * @brief Register a no-argument callback invoked when this component
         *        loses keyboard focus.
         *
         * @param callback Function invoked with no arguments when focus is lost.
         */
        void onBlur(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when this component loses keyboard
         *        focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event when focus is lost.
         */
        void onBlur(std::function<void(const std::optional<sf::Event>&)> callback);
    };

} // namespace ml

#endif // FOCUSABLE_H
