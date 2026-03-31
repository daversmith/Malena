//
// Created by Dave R. Smith on 3/5/25.
//

#include <functional>
#include <utility>
#include <iostream>

#include <Malena/Traits/Communication/Subscribable.h>
#include <Malena/Core/CoreManager.h>
#ifndef SUBSCRIBABLE_TPP
#define SUBSCRIBABLE_TPP
namespace ml
{
    template<typename ENUM_TYPE>
    void Subscribable::subscribe(ENUM_TYPE event, std::function<void(const std::optional<sf::Event> &event)> f)
    {
        Fireable::addCallback(event, this, std::move(f));
    }

    template<typename ENUM_TYPE>
    void Subscribable::subscribe(ENUM_TYPE event, std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(event, this, std::move(cb));
    }

	template<typename ENUM_TYPE>
void Subscribable::publish(ENUM_TYPE event, FilterCallback filter,
	   SystemCallback resolve, SystemCallback reject)
    {
    	static SubscribableDispatcher dispatcher;  // persistent singleton

    	if (filter(*this))
    		EventManager::fire(event, &dispatcher, std::nullopt, resolve, reject);
    }

} // namespace ml
#endif