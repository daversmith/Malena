//
// MessageManager.h
//

#ifndef MALENA_EVENTBUS_H
#define MALENA_EVENTBUS_H

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <algorithm>
#include <optional>
#include <Malena/Managers/DeferredOperationsManager.h>

namespace ml {

class MessageManager : public DeferredOperationsManager<MessageManager> {
private:
    using EventKey = std::tuple<std::type_index, int, std::type_index>;

    struct KeyHash {
        std::size_t operator()(const EventKey& key) const {
            auto h1 = std::get<0>(key).hash_code();
            auto h2 = std::hash<int>{}(std::get<1>(key));
            auto h3 = std::get<2>(key).hash_code();
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    struct Subscription {
        void* subscriber;
        std::function<void(const void*)> callback;
    };

    static std::unordered_map<EventKey, std::vector<Subscription>, KeyHash> subscribers;

    // Internal implementation methods
    static void doUnsubscribe(const EventKey& key, void* subscriber);
    static void doUnsubscribeAll(void* subscriber);

public:
    template<typename DataType, typename Enum>
    static void subscribe(Enum event, void* subscriber, std::function<void(const DataType&)> callback) {
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
    static void publish(Enum event, const DataType& data) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );

        auto it = subscribers.find(key);
        if (it == subscribers.end()) {
            return;
        }

        beginBusy();  // ✅ Using base class method

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

        endBusy();  // ✅ Using base class method - auto-processes pending
    }

    template<typename DataType, typename Enum>
    static void unsubscribe(Enum event, void* subscriber) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );

        deferOrExecute([=]() {  // ✅ Using base class method
            doUnsubscribe(key, subscriber);
        });
    }

    static void unsubscribeAll(void* subscriber);
    static void clear();

    // For plugin unloading - force immediate cleanup
    static void forceUnsubscribeAll(void* subscriber);
};

} // namespace ml

#endif // MALENA_EVENTBUS_H