//
// Created by Dave Smith on 3/20/26.
//

#ifndef CLICKABLE_H
#define CLICKABLE_H


#include <Malena/Traits/Base/Fireable.h>

#include "Malena/Engine/Events/EventDispatcher.h"

namespace ml
{
	class Core;
	class Clickable : public EventReceiver
	{

	public:
		// ── Input events ─────────────────────────────────────────────────────
		void onClick(std::function<void()>);
		void onClick(std::function<void(const std::optional<sf::Event>& event)>);
	};

	class ClickableDispatcher : public EventDispatcher
	{
		inline static EventReceiver* _focused = nullptr;
	public:
		bool occurred(const std::optional<sf::Event> &event) override;
		bool filter(const std::optional<sf::Event> &event, Core *component) override;
		void fire(const std::optional<sf::Event> &event) override;
	};

}
ML_EXPORT(ClickableDispatcher);
#endif //CLICKABLE_H
