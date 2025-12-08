//
// Created by Dave Smith on 12/7/25.
//

#ifndef MALENA_MESSENGER_NEW_TPP
#define MALENA_MESSENGER_NEW_TPP
#include <Malena/Traits/Messenger.h>
namespace ml
{
	template<typename DataType, typename Enum>
	void Messenger::sendMessage(Enum event, const DataType& data) {
		MessageManager::publish(event, data);
	}

	template<typename DataType, typename Enum>
	void Messenger::onMessage(Enum event, std::function<void(const DataType&)> callback) {
		MessageManager::subscribe<DataType>(event, this, callback);
	}

	template<typename DataType, typename Enum>
	void Messenger::offMessage(Enum event) {
		MessageManager::unsubscribe<DataType>(event, this);
	}

 }
#endif