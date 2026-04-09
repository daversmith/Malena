//
// MessageManager.cpp
//

#include <Malena/Engine/Messaging/MessageManager.h>

namespace ml {

    std::unordered_map<MessageManager::EventKey, std::vector<MessageManager::Subscription>, MessageManager::KeyHash>&
    MessageManager::subscribers()
    {
        static std::unordered_map<EventKey, std::vector<Subscription>, KeyHash> s;
        return s;
    }

    void MessageManager::doUnsubscribe(const EventKey& key, void* subscriber) {
        auto it = subscribers().find(key);
        if (it == subscribers().end()) {
            return;
        }

        auto& subs = it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [subscriber](const Subscription& s) { return s.subscriber == subscriber; }),
            subs.end()
        );

        if (subs.empty()) {
            subscribers().erase(it);
        }
    }

    void MessageManager::doUnsubscribeAll(void* subscriber) {
        for (auto it = subscribers().begin(); it != subscribers().end();) {
            auto& subs = it->second;
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                    [subscriber](const Subscription& s) { return s.subscriber == subscriber; }),
                subs.end()
            );

            if (subs.empty()) {
                it = subscribers().erase(it);
            } else {
                ++it;
            }
        }
    }

    void MessageManager::unsubscribeAll(void* subscriber) {
        deferOrExecute([=]() {
            doUnsubscribeAll(subscriber);
        });
    }

    void MessageManager::clear() {
        deferOrExecute([]() {
            subscribers().clear();
            DeferredOperationsManager<MessageManager>::clearPending();
        });
    }

    void MessageManager::forceUnsubscribeAll(void* subscriber) {
        doUnsubscribeAll(subscriber);
    }

} // namespace ml
