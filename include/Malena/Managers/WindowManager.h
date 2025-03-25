//
// Created by Dave Smith on 3/7/25.
//

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>


namespace ml::WindowManager
{
	inline sf::RenderWindow& getWindow()
	{
		/// TODO fix memory leak. is it a problem?
		static sf::RenderWindow* window = new sf::RenderWindow();
		return *window;
	}
} // namespace ml::WindowManager

#endif // WINDOWMANAGER_H
