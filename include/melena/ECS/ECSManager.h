//
// Created by Dave Smith on 3/11/25.
//

#ifndef ECSMANAGER_H
#define ECSMANAGER_H

#pragma once

#include <unordered_map>

#include "EntitiesAndComponents.h"

// Entity is just an integer ID
using Entity = int;

class ECSManager
{
public:
	std::unordered_map<Entity, PositionComponent> positions;
	std::unordered_map<Entity, VelocityComponent> velocities;
	std::unordered_map<Entity, HealthComponent> healths;
	std::unordered_map<Entity, HealthComponent> size;
};


#endif // ECSMANAGER_H
