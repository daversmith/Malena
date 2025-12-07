//
// Created by Dave Smith on 12/7/25.
//

#include <Malena/Events/EventBus.h>

namespace ml {

	// Initialize static member
	std::unordered_map<EventBus::EventKey, std::vector<EventBus::Subscription>, EventBus::KeyHash>
		EventBus::subscribers;

} // namespace ml