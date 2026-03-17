#include <Malena/Engine/Messaging/MessageManager.h>

#ifndef MESSAGE_MANAGER_TPP
#define MESSAGE_MANAGER_TPP
namespace ml
{
	template<typename DataType, typename Enum>
    void MessageManager::subscribe(Enum event, void* subscriber, std::function<void(const DataType&)> callback) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );

        auto wrapper = [callback](const void* data) {
            callback(*static_cast<const DataType*>(data));
        };

        subscribers[key].push_back(Subscription{subscriber, wrapper});
    }

    template<typename DataType, typename Enum>
    void MessageManager::publish(Enum event, const DataType& data) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );

        auto it = subscribers.find(key);
        if (it == subscribers.end()) {
            return;
        }

        beginBusy();  //  Using base class method

        auto subscribersCopy = it->second;

        for (auto& sub : subscribersCopy) {
            auto currentIt = subscribers.find(key);
            if (currentIt != subscribers.end()) {
                auto& currentSubs = currentIt->second;
                auto found = std::find_if(currentSubs.begin(), currentSubs.end(),
                    [&sub](const Subscription& s) {
                        return s.subscriber == sub.subscriber;
                    });

                if (found != currentSubs.end()) {
                    sub.callback(&data);
                }
            }
        }

        endBusy();  //  Using base class method - auto-processes pending
    }

    template<typename DataType, typename Enum>
    void MessageManager::unsubscribe(Enum event, void* subscriber) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );

        deferOrExecute([=]() {  //  Using base class method
            doUnsubscribe(key, subscriber);
        });
    }
}//namespace ml
#endif