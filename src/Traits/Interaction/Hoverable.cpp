//
// Created by Dave Smith on 3/20/26.
//

#include <Malena/Traits/Interaction/Hoverable.h>

#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Traits/State/Flaggable.h>
#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Utilities/MouseEvents.h>

namespace ml
{
	bool HoverableDispatcher::occurred(const std::optional<sf::Event> &event)
	{
		return event->is<sf::Event::MouseMoved>();
	}
	bool HoverableDispatcher::filter(const std::optional<sf::Event> &event, Core *component)
	{
		auto* core = dynamic_cast<Core*>(component);
		if (!core) return false;
		return MouseEvents::isHovered(*core, WindowManager::getWindow());
	}

	void Hoverable::onHover(std::function<void()> f)
	{
		EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
		Fireable::addCallback(Event::HOVER, this, std::move(cb));
	}

	void Hoverable::onHover(std::function<void(const std::optional<sf::Event>&)> f)
	{
		EventCallback cb = std::move(f);
		Fireable::addCallback(Event::HOVER, this, std::move(cb));
	}
	void  Hoverable::onUnhover(std::function<void()> f)
	{
		EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
		Fireable::addCallback(Event::UNHOVER, this, std::move(cb));
	}
	void Hoverable::onUnhover(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		EventCallback cb = std::move(f);
		Fireable::addCallback(Event::UNHOVER, this, std::move(cb));
	}

	void HoverableDispatcher::fire(const std::optional<sf::Event>& event)
	{
		EventManager::fire(Event::HOVER, this, event,
			[](EventReceiver* component, const std::optional<sf::Event>& e)
			{
				// Component is now hovered — set flag
				auto* core = dynamic_cast<Core*>(component);
				if (core) core->enableFlag(Flag::HOVERED);
			},
			[](EventReceiver* component, const std::optional<sf::Event>& e)
			{
				// Component is not hovered — fire unhover only if it WAS hovered
				auto* core = dynamic_cast<Core*>(component);
				if (!core) return;
				if (core->checkFlag(Flag::HOVERED))
				{
					core->disableFlag(Flag::HOVERED);
					component->process(Event::UNHOVER, e);
				}
			});
	}
}

