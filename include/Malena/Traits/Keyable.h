// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Keyable.h
//

#pragma once
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <optional>
namespace ml
{
    /**
     * @brief Trait that adds keyboard-input callbacks to any @c Core object.
     * @ingroup Traits
     *
     * @c Keyable is automatically inherited by every @c ml::Core object.
     * Keyboard callbacks only fire on the component that currently has focus
     * (see @c Focusable). A component receives no keyboard events unless it
     * has been clicked or focus has been explicitly transferred to it.
     *
     * ### Usage
     * @code
     * // React to any key press
     * myInput.onKeypress([](const std::optional<sf::Event>& e){
     *     if (auto* kp = e->getIf<sf::Event::KeyPressed>())
     *         if (kp->code == sf::Keyboard::Key::Enter)
     *             submit();
     * });
     *
     * // React to typed characters (unicode-aware)
     * myInput.onTextEntered([](const std::optional<sf::Event>& e){
     *     if (auto* te = e->getIf<sf::Event::TextEntered>())
     *         buffer += static_cast<char>(te->unicode);
     * });
     *
     * // React to key release
     * myInput.onKeyRelease([]{ /* key released *\/ });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myInput.unsubscribe(ml::Event::KEYPRESS);
     * myInput.unsubscribe(ml::Event::KEY_RELEASE);
     * myInput.unsubscribe(ml::Event::TEXT_ENTERED);
     * @endcode
     *
     * @see Focusable, ml::Event::KEYPRESS, ml::Event::KEY_RELEASE,
     *      ml::Event::TEXT_ENTERED, Unsubscribable
     */
    class Keyable : public EventReceiver
    {
    public:
        // ── Key press ─────────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when a key is pressed
         *        while this component has focus.
         *
         * @param callback Function invoked with no arguments on key press.
         */
        void onKeypress(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when a key is pressed while this
         *        component has focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on key press.
         */
        void onKeypress(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Key release ───────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when a key is released
         *        while this component has focus.
         *
         * @param callback Function invoked with no arguments on key release.
         */
        void onKeyRelease(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when a key is released while this
         *        component has focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on key release.
         */
        void onKeyRelease(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Text entered (unicode) ────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when a unicode character
         *        is entered while this component has focus.
         *
         * Prefer @c onTextEntered over @c onKeypress for text input — it handles
         * modifier keys, input method editors (IME), and non-ASCII characters
         * correctly.
         *
         * @param callback Function invoked with no arguments when text is entered.
         */
        void onTextEntered(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when a unicode character is entered
         *        while this component has focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event when text is entered.
         */
        void onTextEntered(std::function<void(const std::optional<sf::Event>&)> callback);
    };

    /// @cond INTERNAL
    /**
     * @brief Dispatcher singleton for keyboard events.
     *
     * Fires @c KEYPRESS, @c KEY_RELEASE, and @c TEXT_ENTERED to the currently
     * focused component. Registered automatically via @c ML_EXPORT(KeyableDispatcher).
     */
    class KeyableDispatcher : public EventDispatcher
    {
    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };
    /// @endcond

} // namespace ml

ML_EXPORT(KeyableDispatcher)
