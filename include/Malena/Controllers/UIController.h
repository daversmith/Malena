#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#pragma once

#include <Malena/Adapters/CoreAdapter.h>
#include <Malena/Controllers/Controller.h>

namespace ml
{
	class UIController : public Controller
	{
		CoreAdapter proxy;

	public:
		using Controller::Controller;

		void initialization() override = 0;

		void registerEvents() override = 0;

		void onUpdate(std::function<void()> f);



		~UIController() override = default;
	};
} // namespace ml


#endif // GAMELOGIC_H
