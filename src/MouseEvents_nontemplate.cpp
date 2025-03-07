//
// Created by Dave R. Smith on 3/3/25.
//
#include "MouseEvents.h"
bool MouseEvents::isHovered(const sf::FloatRect& bounds,
    const sf::RenderWindow& window)
{
    return bounds.contains(
        (sf::Vector2f) sf::Mouse::getPosition(window)
    );
}

bool MouseEvents::isClicked(const sf::FloatRect& bounds,
        const sf::RenderWindow& window)
{
  return isHovered(bounds, window) && isButtonPressed(sf::Mouse::Button::Left);
}