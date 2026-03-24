//
// Created by Dave Smith on 3/20/26.
//
#include <Malena/Traits/Clickable.h>

#include "Malena/Core/Core.h"
#include "Malena/Engine/Events/EventsManager.h"
#include "Malena/Engine/Window/WindowManager.h"
#include "Malena/Utilities/MouseEvents.h"


bool ml::ClickableDispatcher::occurred(const std::optional<sf::Event> &event)
{
	return sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}
bool ml::ClickableDispatcher::filter(const std::optional<sf::Event> &event, Core *component)
{
	auto* positionable = dynamic_cast<Positionable*>(component);
	if (!positionable) return false;
	return MouseEvents::isClicked(*positionable, WindowManager::getWindow());
}

void ml::Clickable::onClick(std::function<void()> f)
{
	EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
	Fireable::addCallback(Event::CLICK, this, std::move(cb));
}

void ml::Clickable::onClick(std::function<void(const std::optional<sf::Event>&)> f)
{
	EventCallback cb = std::move(f);
	Fireable::addCallback(Event::CLICK, this, std::move(cb));
}
void ml::ClickableDispatcher::fire(const std::optional<sf::Event>& event)
{
	EventsManager::fire(Event::CLICK, this, event,
		[this](EventReceiver* component, const std::optional<sf::Event>& e)
		{
			auto* core = dynamic_cast<Core*>(component);
			if (!core) return;

			// If something else was focused, blur it first
			if (_focused && _focused != component)
			{
				auto* focusedCore = dynamic_cast<Core*>(_focused);
				if (focusedCore) focusedCore->disableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(_focused);
				if (focusable) focusable->process(Event::BLUR, e);

				_focused = nullptr;
			}

			// Focus this component if not already focused
			if (!core->checkFlag(Flag::FOCUSED))
			{
				core->enableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(core);
				if (focusable) focusable->process(Event::FOCUS, e);

				_focused = component;
			}
		},
		[this](EventReceiver* component, const std::optional<sf::Event>& e)
		{
			// Clicked elsewhere — blur if focused
			if (component == _focused)
			{
				auto* core = dynamic_cast<Core*>(component);
				if (core) core->disableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(component);
				if (focusable) focusable->process(Event::BLUR, e);

				_focused = nullptr;
			}
		});
}
