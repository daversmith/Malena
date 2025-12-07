//
// Created by Dave R. Smith on 3/5/25.
//

#include <functional>
#include <utility>

#include <Malena/Managers/ComponentsManager.h>
#include <Malena/Traits/EventPublisher.h>

namespace ml
{
	void EventPublisher::onClick(std::function<void()> f)
	{
		subscribe("click", std::move(f));
	}
	void EventPublisher::onClick(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("click", std::move(f));
	}
	void EventPublisher::onUpdate(std::function<void()> f)
	{
		subscribe("update", std::move(f));
	}
	void EventPublisher::onUpdate(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("update", std::move(f));
	}
	void EventPublisher::onHover(std::function<void()> f)
	{
		subscribe("hover", std::move(f));
	}
	void EventPublisher::onHover(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("hover", std::move(f));
	}
	void EventPublisher::onUnhover(std::function<void()> f)
	{
		subscribe("unhover", f);
	}
	void EventPublisher::onUnhover(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("unhover", f);
	}

	void EventPublisher::onTextEntered(const std::function<void(const std::optional<sf::Event> &event)> &f)
	{
		subscribe("textentered", f);
	}

	void EventPublisher::onTextEntered(std::function<void()> f)
	{

		subscribe("textentered", std::move(f));
	}

	void EventPublisher::onFocus(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("focus", std::move(f));
	}

	void EventPublisher::onFocus(std::function<void()> f)
	{
		subscribe("focus", std::move(f));
	}

	void EventPublisher::onBlur(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("blur", std::move(f));
	}

	void EventPublisher::onBlur(std::function<void()> f)
	{
		subscribe("blur", std::move(f));
	}

	void EventPublisher::onKeypress(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("keypress", std::move(f));
	}

	void EventPublisher::onKeypress(std::function<void()> f)
	{
		subscribe("keypress", std::move(f));
	}

	void EventPublisher::onMouseMoved(std::function<void(const std::optional<sf::Event> &event)> f)
	{
		subscribe("mousemoved", std::move(f));
	}

	void EventPublisher::onMouseMoved(std::function<void()> f)
	{
		subscribe("mousemoved", std::move(f));
	}

	void EventPublisher::subscribe(const std::string &event, std::function<void(const std::optional<sf::Event> &event)> f)
	{
		EventsManager::subscribe(event, reinterpret_cast<UIComponent *>(this), f);
	}

	void EventPublisher::subscribe(const std::string &event, std::function<void()> f)
	{
		auto wrapped = [f](const std::optional<sf::Event> &) { f(); };

		subscribe(event, wrapped);
	}

	void EventPublisher::unsubscribe(const std::string &event)
	{
		EventsManager::unsubscribe(event, reinterpret_cast<UIComponent *>(this));
	}

	void EventPublisher::unsubscribeAll()
	{
		EventsManager::unsubscribeAll(reinterpret_cast<UIComponent *>(this));
	}

	void EventPublisher::publish(const std::string &event, std::function<bool(UIComponent & )> filter)
	{
		EventsManager::fire(event, std::move(filter));
	}


} // namespace ml
