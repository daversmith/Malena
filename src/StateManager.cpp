//
// Created by Dave R. Smith on 3/4/25.
//

#include "StateManager.h"

StateManager::StateManager()
{
    for(int i=0; i<LAST_STATE; i++)
    {
        _states[static_cast<State>(i)] = false;
    }
}

bool StateManager::checkState(State state) const
{
    return _states.at(state);
}
void StateManager::enableState(State state)
{
    _states[state] = true;
}
void StateManager::disableState(State state)
{
    _states[state] = false;
}
void StateManager::toggleState(State state)
{
    _states[state] = !_states[state];
}
void StateManager::setState(State state, bool status)
{
    _states[state] = status;
}