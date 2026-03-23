//
// Created by Dave Smith on 3/21/26.
//

#ifndef CALLBACK_H
#define CALLBACK_H
#include <functional>
#include <SFML/Window/Event.hpp>
namespace ml
{
	class EventReceiver;
	using EventCallback = std::function<void(const std::optional<sf::Event>&)>;
	using SystemCallback = std::function<void(EventReceiver*, const std::optional<sf::Event>&)>;
	using FilterCallback = std::function<bool(EventReceiver&)>;
	using Callback = std::function<void()>;
}
#endif //CALLBACK_H
