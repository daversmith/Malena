//
// Created by Dave R. Smith on 3/3/25.
//
#include <malena/Utilities/MouseEvents.h>

namespace ml
{
	bool MouseEvents::isHovered(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{
		return bounds.contains((sf::Vector2f) sf::Mouse::getPosition(window));
	}

	bool MouseEvents::isClicked(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{
		return isHovered(bounds, window) && isButtonPressed(sf::Mouse::Button::Left);
	}
} // namespace ml
