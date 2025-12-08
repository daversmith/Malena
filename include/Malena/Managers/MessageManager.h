#ifndef MALENA_EVENTBUS_H
#define MALENA_EVENTBUS_H

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <algorithm>

namespace ml {

/**
 * EventBus - Static service for type-safe event messaging
 * Similar to EventsManager but for custom enum-based events
 */
class MessageManager {
private:
    // Key: (enum type, enum value, data type)
    using EventKey = std::tuple<std::type_index, int, std::type_index>;
    
    struct KeyHash {
        std::size_t operator()(const EventKey& key) const {
            auto h1 = std::get<0>(key).hash_code();
            auto h2 = std::hash<int>{}(std::get<1>(key));
            auto h3 = std::get<2>(key).hash_code();
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    
    // Store callbacks with subscriber tracking
    struct Subscription {
        void* subscriber;
        std::function<void(const void*)> callback;
    };
    
    static std::unordered_map<EventKey, std::vector<Subscription>, KeyHash> subscribers;
    
public:
    /**
     * Subscribe to an event
     * @tparam DataType - The data type this event carries
     * @tparam Enum - The event enum type
     * @param event - The specific event value
     * @param subscriber - Pointer to subscribing object (for cleanup)
     * @param callback - Function to call when event fires
     */
    template<typename DataType, typename Enum>
    static void subscribe(Enum event, void* subscriber, std::function<void(const DataType&)> callback) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );
        
        // Wrap the typed callback
        auto wrapper = [callback](const void* data) {
            callback(*static_cast<const DataType*>(data));
        };
        
        subscribers[key].push_back(Subscription{subscriber, wrapper});
    }
    
    /**
     * Publish an event to all subscribers
     * @tparam DataType - The data type being published
     * @tparam Enum - The event enum type
     * @param event - The specific event value
     * @param data - The event data
     */
    template<typename DataType, typename Enum>
    static void publish(Enum event, const DataType& data) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );
        
        auto it = subscribers.find(key);
        if (it != subscribers.end()) {
            for (auto& sub : it->second) {
                sub.callback(&data);
            }
        }
    }
    
    /**
     * Unsubscribe a specific subscriber from an event
     * @tparam DataType - The data type
     * @tparam Enum - The event enum type
     * @param event - The specific event value
     * @param subscriber - Pointer to unsubscribing object
     */
    template<typename DataType, typename Enum>
    static void unsubscribe(Enum event, void* subscriber) {
        auto key = std::make_tuple(
            std::type_index(typeid(Enum)),
            static_cast<int>(event),
            std::type_index(typeid(DataType))
        );
        
        auto it = subscribers.find(key);
        if (it != subscribers.end()) {
            auto& subs = it->second;
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                    [subscriber](const Subscription& s) { return s.subscriber == subscriber; }),
                subs.end()
            );
            
            // Remove key if no subscribers left
            if (subs.empty()) {
                subscribers.erase(it);
            }
        }
    }
    
    /**
     * Unsubscribe from all events for a given subscriber
     * @param subscriber - Pointer to unsubscribing object
     */
    static void unsubscribeAll(void* subscriber) {
        for (auto it = subscribers.begin(); it != subscribers.end();) {
            auto& subs = it->second;
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                    [subscriber](const Subscription& s) { return s.subscriber == subscriber; }),
                subs.end()
            );
            
            if (subs.empty()) {
                it = subscribers.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    /**
     * Clear all subscriptions
     */
    static void clear() {
        subscribers.clear();
    }
};

} // namespace ml

#endif // MALENA_EVENTBUS_H