//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef FLAGGABLE_CPP
#define FLAGGABLE_CPP

#pragma once
#include <Malena/Resources/FlagManager.h>

namespace ml
{
	template<typename StateEnum>
	FlagManager<StateEnum>::FlagManager()
	{
		for (int i = 0; i < static_cast<int>(ml::Flag::LAST_FLAG); i++)
			_states.insert({static_cast<State>(i), false});
	}

	template<typename StateEnum>
	FlagManager<StateEnum>::FlagManager(const FlagManager& stateful)
	{
		for (auto& state : stateful._states)
			_states.insert(state);
	}

	template<typename StateEnum>
	bool FlagManager<StateEnum>::checkFlag(const State state) const
	{
		auto it = _states.find(state);
		return it != _states.end() && it->second;
	}

	template<typename StateEnum>
	void FlagManager<StateEnum>::enableFlag(const State state)
	{
		_states[state] = true;
	}

	template<typename StateEnum>
	void FlagManager<StateEnum>::disableFlag(const State state)
	{
		_states[state] = false;
	}

	template<typename StateEnum>
	void FlagManager<StateEnum>::toggleFlag(const State state)
	{
		_states[state] = !checkFlag(state);
	}

	template<typename StateEnum>
	void FlagManager<StateEnum>::setFlag(const State state, const bool status)
	{
		_states[state] = status;
	}

} // namespace ml
#endif
