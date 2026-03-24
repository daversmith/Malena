//
// Created by Dave Smith on 3/21/26.
//

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#include "Fireable.h"

namespace ml {

class EventDispatcher : public Fireable {
private:
    void fire() final override{};
    bool occurred() final override{};
public:
    void fire(const std::optional<sf::Event> &event) = 0;
    bool occurred(const std::optional<sf::Event> &event) = 0;;
};

} // ml

#endif //EVENTDISPATCHER_H
