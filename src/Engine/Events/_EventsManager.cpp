//
// _EventsManager.cpp
//

#include <Malena/Engine/Events/_EventsManager.h>
#include <Malena/Traits/Base/Fireable.h>
#include <Malena/Core/Core.h>
#include <algorithm>

namespace ml
{
    void _EventsManager::doSubscribe(const std::string& key, EventReceiver* component)
    {
        _subscribers[key].push_back({
            component,
            dynamic_cast<Core*>(component)  // cast once — zero per-frame overhead
        });
    }

    void _EventsManager::doFire(const std::string& key,
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
            if (dispatcher->filter(event, sub.core))
            {
                sub.receiver->process(key, event);
                if (resolve) resolve(sub.receiver, event);
            }
            else
            {
                if (reject) reject(sub.receiver, event);
            }
        }

        endBusy();
    }

    void _EventsManager::unsubscribeAll(Core* core)
    {
        deferOrExecute([core]() { doUnsubscribeAll(core); });
    }

    void _EventsManager::forceUnsubscribeAll(Core* core)
    {
        doUnsubscribeAll(core);
    }

    void _EventsManager::clearAllEvents()
    {
        deferOrExecute([]()
        {
            _subscribers.clear();
            DeferredOperationsManager<_EventsManager>::clearPending();
        });
    }

    void _EventsManager::doUnsubscribe(const std::string& key, Core* core)
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

    void _EventsManager::doUnsubscribeAll(Core* core)
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
