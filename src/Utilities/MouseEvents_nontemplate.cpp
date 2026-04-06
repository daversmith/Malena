//
// Created by Dave R. Smith on 3/3/25.
//
#include <Malena/Utilities/MouseEvents.h>
#include <iostream>
namespace ml
{
	bool MouseEvents::isHovered(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{
		auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		return bounds.contains(mousePos);
	}

	bool MouseEvents::isClicked(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{

		return isHovered(bounds, window) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	}
} // namespace ml
