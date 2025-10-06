//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#pragma once

#include "../Managers/EventsManager.h"
#include "../Managers/FlagManager.h"

namespace ml
{
	class ComponentsManager;
	class Messenger
	{


	public:
		typedef const std::optional<sf::Event> &Event;
		void onClick(std::function<void()>);
		void onUpdate(std::function<void()>);
		void onHover(std::function<void()>);
		void onHover(std::function<void(const std::optional<sf::Event> &event)>);
		void onUnhover(std::function<void()>);
		void onUnhover(std::function<void(const std::optional<sf::Event> &event)>);
		void onClick(std::function<void(const std::optional<sf::Event> &event)>);
		void onUpdate(std::function<void(const std::optional<sf::Event> &event)>);
		void onTextEntered(const std::function<void(const std::optional<sf::Event> &event)> &);
		void onTextEntered(std::function<void()> f);
		void onFocus(std::function<void(const std::optional<sf::Event> &event)>);
		void onFocus(std::function<void()> f);
		void onBlur(std::function<void(const std::optional<sf::Event> &event)>);
		void onBlur(std::function<void()> f);
		void onKeypress(std::function<void(const std::optional<sf::Event> &event)>);
		void onKeypress(std::function<void()> f);
		void onMouseMoved(std::function<void(const std::optional<sf::Event> &event)>);
		void onMouseMoved(std::function<void()> f);
		void subscribe(const std::string &event, std::function<void(const std::optional<sf::Event> &event)>);
		void subscribe(const std::string &event, std::function<void()>);
		void unsubscribe(const std::string &event);
		void unsubscribeAll();
		static void publish(
				const std::string &event,
				std::function<bool(UIComponent &)> filter = [](UIComponent &) { return true; });
	};


} // namespace ml
#endif // MESSAGEMANAGER_H
