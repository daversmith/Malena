//
// Created by Dave Smith on 3/11/25.
//

#ifndef MANAGER_H
#define MANAGER_H

class Manager
{
public:
    virtual ~Manager() = default;

    virtual void handleEvents() = 0;
    virtual void fireEvents() = 0;
    virtual void firePublishedEvents() const = 0;
    //update objects
    virtual void updateComponents() = 0;
    virtual void run() = 0;

    virtual void onUpdate(std::function<void()>) = 0;
    virtual void draw() = 0;

};
#endif //MANAGER_H
