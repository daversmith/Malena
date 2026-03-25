// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/11/25.
//

#ifndef RENDERINGSYSTEM_H
#define RENDERINGSYSTEM_H

#pragma once

#include <SFML/Graphics.hpp>
#include "Core/ECSManager.h"

namespace ml
{
	/**
	 * @brief RenderingSystem.
	 * @ingroup ECSSystem
	 */
	class RenderingSystem
	{
	public:
		void render(sf::RenderWindow &window, ECSManager &ecs)
		{
			int i = 1;
			for (auto &[entity, position] : ecs.positions)
			{
				sf::CircleShape shape(ecs.size[i++].health);
				shape.setPosition({position.x, position.y});
				shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
				window.draw(shape);
			}
		}
	};
}

#endif // RENDERINGSYSTEM_H
