//
// Scrollable.h
//

#pragma once
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <Malena/Engine/Events/EventReceiver.h>

namespace ml
{
    /**
     * @brief Trait that adds mouse-wheel and raw mouse-button callbacks to
     *        any @c Core object.
     * @ingroup Traits
     *
     * @c Scrollable is automatically inherited by every @c ml::Core object.
     * It exposes four event hooks covering the mouse wheel and the low-level
     * mouse button press/release cycle (before click logic is applied):
     *
     * | Callback | Fires when |
     * |----------|-----------|
     * | @c onScroll | Mouse wheel is scrolled while the cursor is over this component |
     * | @c onMouseMoved | Mouse moves anywhere in the window (all components receive this) |
     * | @c onMousePressed | Mouse button is pressed (raw — before click logic) |
     * | @c onMouseReleased | Mouse button is released (raw — before click logic) |
     *
     * ### Usage
     * @code
     * // Scroll a list view on wheel
     * listView.onScroll([&](const std::optional<sf::Event>& e){
     *     if (auto* w = e->getIf<sf::Event::MouseWheelScrolled>())
     *         offset -= w->delta * scrollSpeed;
     * });
     *
     * // Track raw press/release for custom drag logic
     * handle.onMousePressed([]{ dragging = true; });
     * handle.onMouseReleased([]{ dragging = false; });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myComp.unsubscribe(ml::Event::SCROLL);
     * myComp.unsubscribe(ml::Event::MOUSE_MOVED);
     * @endcode
     *
     * @see Clickable, Draggable, ml::Event::SCROLL, ml::Event::MOUSE_MOVED,
     *      ml::Event::MOUSE_PRESSED, ml::Event::MOUSE_RELEASED, Unsubscribable
     */
    class Scrollable : public EventReceiver
    {
    public:
        // ── Mouse wheel ───────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when the mouse wheel
         *        is scrolled while the cursor is over this component.
         *
         * @param callback Function invoked with no arguments on scroll.
         */
        void onScroll(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the mouse wheel is scrolled
         *        while the cursor is over this component, receiving the SFML event.
         *
         * @param callback Function invoked with the SFML event on scroll.
         */
        void onScroll(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Mouse moved (fires for all components regardless of hover) ─────────

        /**
         * @brief Register a no-argument callback invoked whenever the mouse
         *        moves anywhere in the window.
         *
         * Unlike @c onHover, this fires for every component regardless of
         * whether the cursor is over it.
         *
         * @param callback Function invoked with no arguments on mouse move.
         */
        void onMouseMoved(std::function<void()> callback);

        /**
         * @brief Register a callback invoked whenever the mouse moves anywhere
         *        in the window, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on mouse move.
         */
        void onMouseMoved(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Mouse button pressed (raw — before click logic) ───────────────────

        /**
         * @brief Register a no-argument callback invoked when a mouse button
         *        is pressed anywhere in the window.
         *
         * This fires before click logic is evaluated. Prefer @c onClick for
         * standard click handling.
         *
         * @param callback Function invoked with no arguments on mouse press.
         */
        void onMousePressed(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when a mouse button is pressed,
         *        receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on mouse press.
         */
        void onMousePressed(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Mouse button released (raw — before click logic) ──────────────────

        /**
         * @brief Register a no-argument callback invoked when a mouse button
         *        is released anywhere in the window.
         *
         * This fires before click logic is evaluated. Prefer @c onClick for
         * standard click handling.
         *
         * @param callback Function invoked with no arguments on mouse release.
         */
        void onMouseReleased(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when a mouse button is released,
         *        receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on mouse release.
         */
        void onMouseReleased(std::function<void(const std::optional<sf::Event>&)> callback);
    };

    /// @cond INTERNAL
    /**
     * @brief Dispatcher singleton for mouse-wheel and mouse-movement events.
     *
     * Fires @c SCROLL, @c MOUSE_MOVED, @c MOUSE_PRESSED, and @c MOUSE_RELEASED.
     * Registered automatically via @c ML_EXPORT(ScrollableDispatcher).
     */
    class ScrollableDispatcher : public EventDispatcher
    {
    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };
    /// @endcond

} // namespace ml

ML_EXPORT(ScrollableDispatcher)
