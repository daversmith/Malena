//
// Created by Dave Smith on 3/11/25.
//

#ifndef ENTITIESANDCOMPONENTS_H
#define ENTITIESANDCOMPONENTS_H

#pragma once

// Define an Entity type (just an ID)
using Entity = int;

// Component store only data
struct PositionComponent
{
	float x, y;
};

struct VelocityComponent
{
	float vx, vy;
};

struct HealthComponent
{
	int health;
};

struct SizeComponent
{
	float radius;
};

#endif // ENTITIESANDCOMPONENTS_H
