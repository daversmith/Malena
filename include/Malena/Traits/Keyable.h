//
// Keyable.h
//

#pragma once
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Traits/Base/Fireable.h>
#include <Malena/Engine/Events/EventDispatcher.h>
namespace ml
{
	/**
	 * @brief Receiver trait for keyboard input events.
	 * @ingroup Traits
	 *
	 * Inherit through @c Core. Callbacks only fire on the focused component.
	 */
	class Keyable : public EventReceiver
	{
	public:
		// ── Key press ─────────────────────────────────────────────────────────
		void onKeypress(std::function<void()> f);
		void onKeypress(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Key release ───────────────────────────────────────────────────────
		void onKeyRelease(std::function<void()> f);
		void onKeyRelease(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Text entered (unicode) ────────────────────────────────────────────
		void onTextEntered(std::function<void()> f);
		void onTextEntered(std::function<void(const std::optional<sf::Event>&)> f);
	};

	/**
	 * @brief Dispatcher singleton for keyboard events.
	 * @ingroup Traits
	 *
	 * Fires @c "keypress", @c "keyrelease", and @c "textentered" to the
	 * currently focused component. Registered via @c ML_EXPORT(KeyableDispatcher).
	 */
	class KeyableDispatcher : public EventDispatcher
	{
	public:
		bool occurred(const std::optional<sf::Event>& event) override;
		bool filter(const std::optional<sf::Event> &event, Core *component) override;
		void fire(const std::optional<sf::Event>& event) override;
	};

} // namespace ml

ML_EXPORT(KeyableDispatcher)