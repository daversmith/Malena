//
// EventReceiver.cpp
//

#include <Malena/Engine/Events/EventReceiver.h>

namespace ml
{
    void EventReceiver::process(const std::string& key,
                                 const std::optional<sf::Event>& event)
    {
        auto it = _callbacks.find(key);
        if (it == _callbacks.end()) return;
        for (auto& cb : it->second)
            cb(event);
    }

    std::vector<EventCallback>& EventReceiver::getCallbacks(const std::string& key)
    {
        return _callbacks[key];
    }

} // namespace ml
