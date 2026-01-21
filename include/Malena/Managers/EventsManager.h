//
// EventsManager.h
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include <any>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <Malena/Managers/DeferredOperationsManager.h>

namespace ml
{
	class UIComponent;
	class Controller;

	class EventsManager : public DeferredOperationsManager<EventsManager>
	{
	private:
		inline static std::map<std::string,
							   std::map<ml::UIComponent *, std::function<void(const std::optional<sf::Event> &event)>>>
				events;
		inline static bool clearPending = false;

	public:
		static std::optional<std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &event)>> *>
		getEvent(const std::string &event);

		static void setEvent(const std::string &event);

		// Main API
		static void fire(
				const std::string &event,
				std::function<bool(UIComponent &)> filter = [](UIComponent &) { return true; },
				std::function<void()> system = nullptr,
				const std::optional<sf::Event> &e = std::nullopt);

		static void subscribe(const std::string &event, UIComponent *component,
							  std::function<void(const std::optional<sf::Event> &)> callback);

		static void unsubscribe(const std::string &event, UIComponent *component);
		static void unsubscribeAll(UIComponent *component);
		static void clearAllEvents();

		// For plugin unloading - force immediate cleanup
		static void forceUnsubscribeAll(UIComponent *component);

	private:
		// Internal methods for actual removal
		static void doUnsubscribe(const std::string &event, UIComponent *component);
		static void doUnsubscribeAll(UIComponent *component);
	};
} // namespace ml

#endif // EVENTSMANAGER_H