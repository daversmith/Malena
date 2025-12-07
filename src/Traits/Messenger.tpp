//
// Created by Dave Smith on 12/7/25.
//
#include <Malena/Traits/Messenger.h>
namespace ml
{
	template<typename DataType, typename Enum>
	void Messenger::sendMessage(Enum event, const DataType& data) {
		EventBus::publish(event, data);
	}

	template<typename DataType, typename Enum>
	void Messenger::onMessage(Enum event, std::function<void(const DataType&)> callback) {
		EventBus::subscribe<DataType>(event, this, callback);
	}

	template<typename DataType, typename Enum>
	void Messenger::offMessage(Enum event) {
		EventBus::unsubscribe<DataType>(event, this);
	}

	void Messenger::offAllMessages() {
			EventBus::unsubscribeAll(this);
		}

	Messenger::~Messenger() {
		// Auto-cleanup when object is destroyed
		offAllMessages();
	}
 }