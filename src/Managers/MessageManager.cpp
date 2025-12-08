//
// Created by Dave Smith on 12/7/25.
//

#include <Malena/Managers/MessageManager.h>

namespace ml {

	// Initialize static member
	std::unordered_map<MessageManager::EventKey, std::vector<MessageManager::Subscription>, MessageManager::KeyHash>
		MessageManager::subscribers;

} // namespace ml