//
// Created by Dave Smith on 3/22/26.
//

#ifndef FRAMEDISPATCHER_H
#define FRAMEDISPATCHER_H
#include "Malena/Traits/Base/Fireable.h"


class FrameDispatcher : public ml::Fireable {
public:
    FrameDispatcher() : Fireable(DispatchType::FRAME){}
    void fire() override = 0;
    bool occurred() override = 0;
private:
    void fire(const std::optional<sf::Event> &event) final override{};
    bool occurred(const std::optional<sf::Event> &event)  final override{};
};



#endif //FRAMEDISPATCHER_H
