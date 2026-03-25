//
// Created by Dave Smith on 3/21/26.
//

#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>
#include <SFML/Window/Event.hpp>
#include <optional>
namespace ml
{
	class EventReceiver;

	/**
	 * @brief Callback type for event handlers that receive the raw SFML event.
	 * @ingroup EngineEvents
	 *
	 * Used as the parameter type when subscribing via @c subscribe(event, EventCallback).
	 * The @c std::optional<sf::Event> is non-empty for input events and
	 * @c std::nullopt for frame (@c UPDATE) events.
	 *
	 * @code
	 * ml::EventCallback cb = [](const std::optional<sf::Event>& e) {
	 *     if (auto* kp = e->getIf<sf::Event::KeyPressed>())
	 *         std::cout << "key pressed\n";
	 * };
	 * myRect.subscribe(ml::Event::KEYPRESS, cb);
	 * @endcode
	 */
	using EventCallback = std::function<void(const std::optional<sf::Event>&)>;

	/**
	 * @brief Callback type for no-argument event handlers.
	 * @ingroup EngineEvents
	 *
	 * Convenience alias accepted by all trait subscribe overloads that
	 * do not need the raw SFML event.
	 *
	 * @code
	 * ml::Callback cb = []{ std::cout << "clicked\n"; };
	 * myRect.onClick(cb);
	 * @endcode
	 */
	using Callback = std::function<void()>;

	/// @cond INTERNAL
	using SystemCallback = std::function<void(EventReceiver*, const std::optional<sf::Event>&)>;
	using FilterCallback = std::function<bool(EventReceiver&)>;
	/// @endcond

} // namespace ml

#endif // CALLBACK_H
