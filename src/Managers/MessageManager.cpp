//
// MessageManager.cpp
//

#include <Malena/Managers/MessageManager.h>

namespace ml {

    std::unordered_map<MessageManager::EventKey, std::vector<MessageManager::Subscription>, MessageManager::KeyHash>
        MessageManager::subscribers;

    void MessageManager::doUnsubscribe(const EventKey& key, void* subscriber) {
        auto it = subscribers.find(key);
        if (it == subscribers.end()) {
            return;
        }

        auto& subs = it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [subscriber](const Subscription& s) { return s.subscriber == subscriber; }),
            subs.end()
        );

        if (subs.empty()) {
            subscribers.erase(it);
        }
    }

    void MessageManager::doUnsubscribeAll(void* subscriber) {
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

    void MessageManager::unsubscribeAll(void* subscriber) {
        deferOrExecute([=]() {  // ✅ Using base class method
            doUnsubscribeAll(subscriber);
        });
    }

    void MessageManager::clear() {
        deferOrExecute([]() {  // ✅ Using base class method
            subscribers.clear();
            DeferredOperationsManager<MessageManager>::clearPending();
        });
    }

    void MessageManager::forceUnsubscribeAll(void* subscriber) {
        // Force immediate - bypass deferral (for plugin unloading)
        doUnsubscribeAll(subscriber);
    }

} // namespace ml