//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef FLAGGABLE_H
#define FLAGGABLE_H

#pragma once

#include <map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>

#include "Malena/Utilities/Flag.h"

namespace ml
{
	template <typename StateEnum>
	class FlagManager
	{
	public:
		using State = StateEnum;

	private:
		std::map<State, bool> _states;

	public:
		FlagManager();
		FlagManager(const FlagManager& stateful);
		bool checkFlag(State state) const;
		void enableFlag(State state);
		void disableFlag(State state);
		void toggleFlag(State state);
		void setFlag(State state, bool status);

	};

} // namespace ml
#include "../../../src/Traits/FlagManager.cpp"

#endif // FLAGGABLE_H
