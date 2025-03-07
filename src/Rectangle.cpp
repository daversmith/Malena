//
// Created by Dave R. Smith on 3/4/25.
//

#include "Rectangle.h"
#include <iostream>

void Rectangle::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::RectangleShape r = *this;
    target.draw(r);
}

void Rectangle::update()
{
     if(checkState(StateManager::HOVERED))
        sf::RectangleShape::setFillColor(sf::Color::Blue);
     else
        sf::RectangleShape::setFillColor(sf::Color::Magenta);
}

void Rectangle::eventHandler()
{
    setState(StateManager::HOVERED,
        MouseEvents::isHovered(*this, Window::window));
}

sf::FloatRect  Rectangle::getGlobalBounds() const
{
    return sf::RectangleShape::getGlobalBounds();
}
