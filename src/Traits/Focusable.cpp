//
// Created by Dave Smith on 3/21/26.
//

#include "../../include/Malena/Traits/Focusable.h"

#include <iostream>

#include "../../include/Malena/Engine/Events/Fireable.h"

namespace ml {
	// Focusable.cpp
	void Focusable::onFocus(std::function<void()> f)
	{
		EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
		Fireable::addCallback(Event::FOCUS, this, std::move(cb));
	}
	void Focusable::onFocus(std::function<void(const std::optional<sf::Event>&)> f)
	{
		Fireable::addCallback(Event::FOCUS, this, std::move(f));
	}

	void Focusable::onBlur(std::function<void()> f)
	{
		EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
		Fireable::addCallback(Event::BLUR, this, std::move(cb));
	}
	void Focusable::onBlur(std::function<void(const std::optional<sf::Event>&)> f)
	{
		Fireable::addCallback(Event::BLUR, this, std::move(f));
	}
} // ml