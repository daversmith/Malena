//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef STATEFUL_H
#define STATEFUL_H

#include <map>

namespace ml {

class Stateful {
public:
    enum State
    {
        HOVERED, CLICKED, HIDDEN, BLINKING,
        FOCUSED,
        //// do not add states below
        LAST_STATE
    };
private:
    std::map<State, bool> _states;
public:
    Stateful();
    bool checkState(State state) const;
    void enableState(State state);
    void disableState(State state);
    void toggleState(State state);
    void setState(State state, bool status);
};


} // namespace ml
#endif //STATEMANAGER_H


