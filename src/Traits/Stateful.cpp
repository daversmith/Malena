//
// Created by Dave R. Smith on 3/4/25.
//

#include <Malena/Traits/Stateful.h>

namespace ml
{
	Stateful::Stateful()
	{
		for (int i = 0; i < LAST_STATE; i++)
		{
			// _states[static_cast<State>(i)] = false;
			_states.insert({static_cast<State>(i), false});
		}
	}

	bool Stateful::checkState(const State state) const
	{
		return _states.at(state);
	}

	void Stateful::enableState(const State state)
	{
		_states.at(state)= true;
	}

	void Stateful::disableState(const State state)
	{
		_states.at(state) = false;
	}

	void Stateful::toggleState(const State state)
	{
		_states.at(state) = !_states.at(state);
	}

	void Stateful::setState(const State state, const bool status)
	{
		_states.at(state) = status;
	}
} // namespace ml
