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
			_states[static_cast<State>(i)] = false;
		}
	}

	bool Stateful::checkState(const State state) const
	{
		return _states.at(state);
	}

	void Stateful::enableState(const State state)
	{
		_states[state] = true;
	}

	void Stateful::disableState(const State state)
	{
		_states[state] = false;
	}

	void Stateful::toggleState(const State state)
	{
		_states[state] = !_states[state];
	}

	void Stateful::setState(const State state, const bool status)
	{
		_states[state] = status;
	}
} // namespace ml
