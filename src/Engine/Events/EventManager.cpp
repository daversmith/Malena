//
// _EventsManager.cpp
//

#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Core/Core.h>
#include <algorithm>

namespace ml
{
    void EventManager::doSubscribe(const std::string& key, EventReceiver* component, Core* core)
    {
    	auto& subs = _subscribers[key];
    	for (const auto& sub : subs)
    		if (sub.receiver == component) return;  // already subscribed

    	// Only reach for RTTI when the caller could not tell us. A caller that
    	// subscribes from inside its own constructor MUST pass `core` — see the
    	// note on subscribe().
    	if (!core) core = dynamic_cast<Core*>(component);
    	subs.push_back({component, core});
    }

    void EventManager::doFire(const std::string& key,
                                 Fireable* dispatcher,
                                 const std::optional<sf::Event>& event,
                                 SystemCallback resolve,
                                 SystemCallback reject)
    {
        auto it = _subscribers.find(key);
        if (it == _subscribers.end()) return;

        beginBusy();

        // Copy — safe against unsubscribe calls during iteration
        auto copy = it->second;

        for (auto& sub : copy)
        {
            auto liveIt = _subscribers.find(key);
            if (liveIt == _subscribers.end()) break;

            bool alive = std::any_of(liveIt->second.begin(), liveIt->second.end(),
                [&sub](const Subscriber& s){ return s.receiver == sub.receiver; });
            if (!alive) continue;   // freed/unsubscribed by an earlier handler this dispatch

            if (dispatcher->filter(event, sub.core))
            {
                sub.receiver->process(key, event);

                // process() (the receiver's own handler) may have DESTROYED this very
                // receiver — self-freeing clicks are legitimate (a delete/close button
                // that removes its own row, a click that rebuilds its widget pool). The
                // resolve callback (focus/blur bookkeeping) would then dereference freed
                // memory: EXC_BAD_ACCESS in dynamic_cast inside ClickableDispatcher::fire.
                // Re-check liveness against the live list before running it.
                auto postIt = _subscribers.find(key);
                const bool stillAlive = postIt != _subscribers.end() &&
                    std::any_of(postIt->second.begin(), postIt->second.end(),
                        [&sub](const Subscriber& s){ return s.receiver == sub.receiver; });
                if (stillAlive && resolve) resolve(sub.receiver, event);
            }
            else
            {
                if (reject) reject(sub.receiver, event);
            }
        }

        endBusy();
    }

    void EventManager::unsubscribeAll(Core* core)
    {
        deferOrExecute([core]() { doUnsubscribeAll(core); });
    }

    void EventManager::forceUnsubscribeAll(Core* core)
    {
        doUnsubscribeAll(core);
    }

    bool EventManager::hasReceiver(EventReceiver* receiver)
    {
        if (!receiver) return false;
        for (const auto& [key, subs] : _subscribers)
            for (const auto& s : subs)
                if (s.receiver == receiver) return true;
        return false;
    }

    void EventManager::clear()
    {
        deferOrExecute([]()
        {
            _subscribers.clear();
            DeferredOperationsManager<EventManager>::clearPending();
        });
    }

    void EventManager::doUnsubscribe(const std::string& key, Core* core)
    {
        auto it = _subscribers.find(key);
        if (it == _subscribers.end()) return;

        auto& subs = it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [core](const Subscriber& s){ return s.core == core; }),
            subs.end());

        if (subs.empty())
            _subscribers.erase(it);
    }

    void EventManager::doUnsubscribeAll(Core* core)
    {
        for (auto it = _subscribers.begin(); it != _subscribers.end();)
        {
            auto& subs = it->second;
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                    [core](const Subscriber& s){ return s.core == core; }),
                subs.end());

            if (subs.empty())
                it = _subscribers.erase(it);
            else
                ++it;
        }
    }

} // namespace ml
