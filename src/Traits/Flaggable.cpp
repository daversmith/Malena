//
// Created by Dave R. Smith on 3/4/25.
//


#ifndef FLAGGABLE_CPP
#define FLAGGABLE_CPP

#pragma once
#include <Malena/Traits/Flaggable.h>

namespace ml
{
	template<typename StateEnum>
	Flaggable<StateEnum>::Flaggable()
	{
		for (int i = 0; i < static_cast<int>(ml::State::LAST_STATE); i++)
		{
			// _states[static_cast<State>(i)] = false;
			_states.insert({static_cast<State>(i), false});
		}
	}
	template<typename StateEnum>
	Flaggable<StateEnum>::Flaggable(const Flaggable& stateful)
	{
		for(auto & state : stateful._states)
			_states.insert(state);
	}
	template<typename StateEnum>
	bool Flaggable<StateEnum>::checkState(const State state) const
	{
		return _states.at(state);
	}
	template<typename StateEnum>
	void Flaggable<StateEnum>::enableState(const State state)
	{
		_states.at(state)= true;
	}
	template<typename StateEnum>
	void Flaggable<StateEnum>::disableState(const State state)
	{
		_states.at(state) = false;
	}
	template<typename StateEnum>
	void Flaggable<StateEnum>::toggleState(const State state)
	{
		_states.at(state) = !_states.at(state);
	}
	template<typename StateEnum>
	void Flaggable<StateEnum>::setState(const State state, const bool status)
	{
		_states.at(state) = status;
	}


} // namespace ml
#endif