//
// Created by Dave R. Smith on 3/4/25.
//


#ifndef FLAGGABLE_CPP
#define FLAGGABLE_CPP

#pragma once
#include <Malena/Managers/FlagManager.h>

namespace ml
{
	template<typename StateEnum>
	FlagManager<StateEnum>::FlagManager()
	{
		for (int i = 0; i < static_cast<int>(ml::Flag::LAST_STATE); i++)
		{
			// _states[static_cast<Flag>(i)] = false;
			_states.insert({static_cast<State>(i), false});
		}
	}
	template<typename StateEnum>
	FlagManager<StateEnum>::FlagManager(const FlagManager& stateful)
	{
		for(auto & state : stateful._states)
			_states.insert(state);
	}
	template<typename StateEnum>
	bool FlagManager<StateEnum>::checkFlag(const State state) const
	{
		return _states.at(state);
	}
	template<typename StateEnum>
	void FlagManager<StateEnum>::enableFlag(const State state)
	{
		_states.at(state)= true;
	}
	template<typename StateEnum>
	void FlagManager<StateEnum>::disableFlag(const State state)
	{
		_states.at(state) = false;
	}
	template<typename StateEnum>
	void FlagManager<StateEnum>::toggleFlag(const State state)
	{
		_states.at(state) = !_states.at(state);
	}
	template<typename StateEnum>
	void FlagManager<StateEnum>::setFlag(const State state, const bool status)
	{
		_states.at(state) = status;
	}


} // namespace ml
#endif