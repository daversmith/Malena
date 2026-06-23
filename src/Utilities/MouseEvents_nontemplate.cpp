//
// Created by Dave R. Smith on 3/3/25.
//
#include <Malena/Utilities/MouseEvents.h>
#include <iostream>
#include <optional>
namespace ml
{
	// Testing override: when set, isHovered() uses this window-pixel position
	// instead of the live OS cursor (see MouseEvents::setTestCursor).
	namespace { std::optional<sf::Vector2i> g_testCursor; }

	void MouseEvents::setTestCursor(std::optional<sf::Vector2i> pixelPos)
	{
		g_testCursor = pixelPos;
	}

	bool MouseEvents::isHovered(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{
		const sf::Vector2i pixel = g_testCursor ? *g_testCursor : sf::Mouse::getPosition(window);
		auto mousePos = window.mapPixelToCoords(pixel);
		return bounds.contains(mousePos);
	}

	bool MouseEvents::isClicked(const sf::FloatRect &bounds, const sf::RenderWindow &window)
	{

		return isHovered(bounds, window) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	}
} // namespace ml
