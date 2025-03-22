//
// Created by Dave Smith on 3/11/25.
//

#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H

#pragma once

#include <iostream>

#include "ECSManager.h"
#include "EventManagerECS.h"


class HealthSystem
{
public:
	void applyDamage(EventManagerECS &eventManager, ECSManager &ecs, Entity entity, int damage)
	{
		if (ecs.healths.find(entity) != ecs.healths.end())
		{
			eventManager.pushEvent([&, entity, damage]() {
				ecs.healths[entity].health -= damage;
				std::cout << "Entity " << entity << " took " << damage
						  << " damage! Health: " << ecs.healths[entity].health << std::endl;

				if (ecs.healths[entity].health <= 0)
				{
					std::cout << "Entity " << entity << " has died!\n";
					ecs.healths.erase(entity);
					ecs.positions.erase(entity);
					ecs.velocities.erase(entity);
				}
			});
		}
	}
};


#endif // HEALTHSYSTEM_H
