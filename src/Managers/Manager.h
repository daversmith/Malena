//
// Created by Dave Smith on 3/11/25.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <functional>

namespace ml {

class Manager
{
public:
    virtual ~Manager() = default;

    virtual void handleEvents() = 0;
    virtual void fireEvents(const std::optional<sf::Event> &event) = 0;
    //update objects
    virtual void updateComponents() = 0;
    virtual void run() = 0;

    // virtual void onUpdate(std::function<void()>) = 0;
    virtual void draw() = 0;

};
} // namespace ml
#endif //MANAGER_H


