//
// Created by Dave Smith on 3/20/26.
//

#ifndef HOVERABLE_H
#define HOVERABLE_H
#include <Malena/Engine/Events/EventReceiver.h>

#include <Malena/Engine/Events/Fireable.h>

#include <Malena/Engine/Events/EventDispatcher.h>
namespace ml {

class Hoverable : public EventReceiver
{
public:
	void onHover(std::function<void()> f);
	void onHover(std::function<void(const std::optional<sf::Event> &event)> f);
	void onUnhover(std::function<void()> f);
	void onUnhover(std::function<void(const std::optional<sf::Event> &event)> f);


};

	class HoverableDispatcher : public EventDispatcher
	{

	public:
		bool occurred(const std::optional<sf::Event> &event) override;
		bool filter(const std::optional<sf::Event> &event, Core *component) override;
		void fire(const std::optional<sf::Event> &event) override;
	};
} // ml

ML_EXPORT(HoverableDispatcher)
#endif //HOVERABLE_H
