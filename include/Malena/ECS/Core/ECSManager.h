// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/11/25.
//

#ifndef ECSMANAGER_H
#define ECSMANAGER_H

#pragma once

#include <unordered_map>

#include "EntitiesAndComponents.h"
namespace ml
{
	// Entity is just an integer ID
	using Entity = int;

	/**
	 * @brief ECSManager.
	 * @ingroup ECSCore
	 */
	class ECSManager
	{
	public:
		std::unordered_map<Entity, PositionComponent> positions;
		std::unordered_map<Entity, VelocityComponent> velocities;
		std::unordered_map<Entity, HealthComponent> healths;
		std::unordered_map<Entity, HealthComponent> size;
	};
}

#endif // ECSMANAGER_H
