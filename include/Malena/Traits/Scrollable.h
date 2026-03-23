//
// Scrollable.h
//

#pragma once
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Traits/Base/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
namespace ml
{
	/**
	 * @brief Receiver trait for mouse wheel and mouse movement events.
	 * @ingroup Traits
	 *
	 * Inherit through @c Core. Scroll fires only when the mouse is over
	 * the component. Mouse moved fires for all components.
	 */
	class Scrollable : public EventReceiver
	{
	public:
		// ── Mouse wheel ───────────────────────────────────────────────────────
		void onScroll(std::function<void()> f);
		void onScroll(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Mouse moved (fires for all, regardless of hover) ──────────────────
		void onMouseMoved(std::function<void()> f);
		void onMouseMoved(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Mouse button pressed (raw — before click logic) ───────────────────
		void onMousePressed(std::function<void()> f);
		void onMousePressed(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Mouse button released (raw — before click logic) ──────────────────
		void onMouseReleased(std::function<void()> f);
		void onMouseReleased(std::function<void(const std::optional<sf::Event>&)> f);
	};

	/**
	 * @brief Dispatcher singleton for mouse wheel and mouse movement events.
	 * @ingroup Traits
	 *
	 * Fires @c "scroll", @c "mousemoved", @c "mousepressed", @c "mousereleased".
	 * Registered via @c ML_EXPORT(ScrollableDispatcher).
	 */
	class ScrollableDispatcher : public EventDispatcher
	{
	public:
		bool occurred(const std::optional<sf::Event>& event) override;
		bool filter(const std::optional<sf::Event> &event, Core *component) override;
		void fire(const std::optional<sf::Event>& event) override;
	};

} // namespace ml

ML_EXPORT(ScrollableDispatcher)