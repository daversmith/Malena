//
// Created by Dave R. Smith on 3/3/25.
//

#ifndef MOUSEEVENTS_H
#define MOUSEEVENTS_H

#pragma once

#include <SFML/Graphics.hpp>

namespace ml
{
	class MouseEvents
	{

	public:
		template<typename T>
		static bool isHovered(const T &obj, const sf::RenderWindow &window)
		{
			return isHovered(obj.getGlobalBounds(), window);
		}

		static bool isHovered(const sf::FloatRect &bounds, const sf::RenderWindow &window);

		template<typename T>
		static bool isClicked(const T &obj, const sf::RenderWindow &window)
		{
			return isClicked(obj.getGlobalBounds(), window);
		}

		static bool isClicked(const sf::FloatRect &bounds, const sf::RenderWindow &window);
	};
} // namespace ml

#endif // MOUSEEVENTS_H
