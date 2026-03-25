//
// Updatable.h
//

#pragma once
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/FrameDispatcher.h>
#include <functional>
#include <optional>
#include <SFML/Window/Event.hpp>

namespace ml
{
    /**
     * @brief Trait that adds per-frame update and window lifecycle callbacks
     *        to any @c Core object.
     * @ingroup Traits
     *
     * @c Updatable is automatically inherited by every @c ml::Core object.
     * It provides @c onUpdate for per-frame logic, and three window-level
     * event hooks that fire when the application window changes state.
     *
     * | Callback | Fires when |
     * |----------|-----------|
     * | @c onUpdate | Every frame, before drawing |
     * | @c onWindowResized | The application window is resized |
     * | @c onWindowFocusGained | The application window gains OS focus |
     * | @c onWindowFocusLost | The application window loses OS focus |
     *
     * ### Usage
     * @code
     * // Advance an animation every frame
     * mySprite.onUpdate([&]{
     *     angle += rotationSpeed;
     *     mySprite.setRotation(angle);
     * });
     *
     * // Reflow layout when the window is resized
     * myPanel.onWindowResized([&](const std::optional<sf::Event>& e){
     *     if (auto* r = e->getIf<sf::Event::Resized>())
     *         myPanel.setSize({float(r->size.x), float(r->size.y)});
     * });
     * @endcode
     *
     * To unsubscribe:
     * @code
     * myComp.unsubscribe(ml::Event::UPDATE);
     * myComp.unsubscribe(ml::Event::WINDOW_RESIZED);
     * @endcode
     *
     * @see ml::Event::UPDATE, ml::Event::WINDOW_RESIZED,
     *      ml::Event::WINDOW_FOCUS_GAINED, ml::Event::WINDOW_FOCUS_LOST,
     *      UIController::onUpdate, Unsubscribable
     */
    class Updatable : public EventReceiver
    {
    public:
        // ── Per-frame ─────────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked every frame.
         *
         * The callback fires once per frame before the draw pass. All active
         * components receive this regardless of hover or focus state.
         *
         * @param callback Function invoked with no arguments each frame.
         */
        void onUpdate(std::function<void()> callback);

        /**
         * @brief Register a callback invoked every frame, receiving the SFML event.
         *
         * For frame events the @c std::optional<sf::Event> is always
         * @c std::nullopt — use the no-argument overload unless you specifically
         * need the optional parameter in your callback signature.
         *
         * @param callback Function invoked with the SFML event (always nullopt) each frame.
         */
        void onUpdate(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Window events ─────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when the application
         *        window is resized.
         *
         * @param callback Function invoked with no arguments on window resize.
         */
        void onWindowResized(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the application window is
         *        resized, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event on window resize.
         */
        void onWindowResized(std::function<void(const std::optional<sf::Event>&)> callback);

        /**
         * @brief Register a no-argument callback invoked when the application
         *        window gains OS focus.
         *
         * @param callback Function invoked with no arguments when focus is gained.
         */
        void onWindowFocusGained(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the application window gains
         *        OS focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event when focus is gained.
         */
        void onWindowFocusGained(std::function<void(const std::optional<sf::Event>&)> callback);

        /**
         * @brief Register a no-argument callback invoked when the application
         *        window loses OS focus.
         *
         * @param callback Function invoked with no arguments when focus is lost.
         */
        void onWindowFocusLost(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when the application window loses
         *        OS focus, receiving the raw SFML event.
         *
         * @param callback Function invoked with the SFML event when focus is lost.
         */
        void onWindowFocusLost(std::function<void(const std::optional<sf::Event>&)> callback);
    };

    /// @cond INTERNAL
    class UpdatableDispatcher : public FrameDispatcher
    {
    public:
        bool filter(const std::optional<sf::Event>& event, Core* component) override { return true; }
        void fire() override;
        bool occurred() override { return true; }
    };
    /// @endcond

} // namespace ml

ML_EXPORT(UpdatableDispatcher)
