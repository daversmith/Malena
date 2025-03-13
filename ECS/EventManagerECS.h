//
// Created by Dave Smith on 3/11/25.
//

#ifndef EVENTMANAGERECS_H
#define EVENTMANAGERECS_H



#include <queue>
#include <functional>
#include <unordered_map>

class EventManagerECS {
private:
    std::queue<std::function<void()>> eventQueue;  // Queue of events to process

public:
    void pushEvent(std::function<void()> event) {
        eventQueue.push(event);  // Add event to queue
    }

    void processEvents() {
        while (!eventQueue.empty()) {
            eventQueue.front()();  // Execute event
            eventQueue.pop();  // Remove event from queue
        }
    }
};




#endif //EVENTMANAGERECS_H
