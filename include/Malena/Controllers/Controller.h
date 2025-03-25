//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include "../Interfaces/Updateable.h"
#include "../Managers/ComponentsManager.h"

namespace ml
{
	class Controller : public ComponentsManager
	{
	public:
		virtual void initialization() = 0;

		virtual void registerEvents() = 0;

		~Controller() override = default;
	};
} // namespace ml

#endif // CONTROLLER_H
