//
// Created by Dave Smith on 3/11/25.
//

#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#pragma once

#include <unordered_map>
#include <vector>

#include "ECSManager.h"
#include "EntitiesAndComponents.h"

class MovementSystem
{
public:
	static void update(ECSManager &ecs, const float deltaTime)
	{
		for (auto &[entity, velocity] : ecs.velocities)
		{
			if (ecs.positions.find(entity) != ecs.positions.end())
			{
				ecs.positions[entity].x += velocity.vx * deltaTime;
				ecs.positions[entity].y += velocity.vy * deltaTime;
			}
		}
	}
};

#endif // MOVEMENTSYSTEM_H
