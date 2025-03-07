//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include <map>

class StateManager {
public:
    enum State
    {
        HOVERED, CLICKED,

        //// do not add states below
        LAST_STATE
    };
private:
    std::map<State, bool> _states;
public:
    StateManager();
    bool checkState(State state) const;
    void enableState(State state);
    void disableState(State state);
    void toggleState(State state);
    void setState(State state, bool status);
};



#endif //STATEMANAGER_H
