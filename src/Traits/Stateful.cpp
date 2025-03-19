//
// Created by Dave R. Smith on 3/4/25.
//

#include "../Traits/Stateful.h"

namespace ml {

Stateful::Stateful()
{
    for(int i=0; i<LAST_STATE; i++)
    {
        _states[static_cast<State>(i)] = false;
    }
}

bool Stateful::checkState(State state) const
{
    return _states.at(state);
}
void Stateful::enableState(State state)
{
    _states[state] = true;
}
void Stateful::disableState(State state)
{
    _states[state] = false;
}
void Stateful::toggleState(State state)
{
    _states[state] = !_states[state];
}
void Stateful::setState(State state, bool status)
{
    _states[state] = status;
}} // namespace ml

