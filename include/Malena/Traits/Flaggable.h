//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef FLAGGABLE_H
#define FLAGGABLE_H

#pragma once

#include <map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>

#include "Malena/Utilities/State.h"

namespace ml
{
	template <typename StateEnum>
	class Flaggable
	{
	public:
		using State = StateEnum;

	private:
		std::map<State, bool> _states;

	public:
		Flaggable();
		Flaggable(const Flaggable& stateful);
		bool checkState(State state) const;
		void enableState(State state);
		void disableState(State state);
		void toggleState(State state);
		void setState(State state, bool status);

	};

} // namespace ml
#include "../../../src/Traits/Flaggable.cpp"

#endif // FLAGGABLE_H
