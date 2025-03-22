//
// Created by Dave Smith on 3/7/25.
//

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H


#include <SFML/Graphics.hpp>

namespace ml
{

	namespace WindowManager
	{
		inline sf::RenderWindow &getWindow()
		{
			/// TODO fix memory leak. is it a problem?
			static sf::RenderWindow *window = new sf::RenderWindow();
			return *window;
		}

	} // namespace WindowManager
} // namespace ml
#endif // WINDOWMANAGER_H
