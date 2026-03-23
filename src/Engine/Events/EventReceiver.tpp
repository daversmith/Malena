#include <Malena/Engine/Events/EventReceiver.h>

#include "Malena/Utilities/EnumKey.h"
#ifndef EVENTSRECEIVER_TPP
#define EVENTSRECEIVER_TPP
namespace ml
{
	template <typename ENUM_TYPE>
	void EventReceiver::process(ENUM_TYPE eventName, const std::optional<sf::Event>& event)
	{
		auto evIt = _callbacks.find(EnumKey::get(eventName));
		if (evIt == _callbacks.end()) return;

		for (auto& cb : evIt->second)
			cb(event);
	}
}
#endif