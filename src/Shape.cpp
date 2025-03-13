//
// Created by Dave R. Smith on 3/4/25.
//
#ifndef SHAPE_CPP
#define SHAPE_CPP

#include "Shape.h"
#include <iostream>

template <typename T>
void Shape<T>::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    T r = *this;
    target.draw(r);
}
template <typename T>
void Shape<T>::update()
{
     if(StateManager::checkState(StateManager::HOVERED))
        sf::RectangleShape::setFillColor(sf::Color::Blue);
     else
        sf::RectangleShape::setFillColor(sf::Color::Magenta);
}
template <typename T>
void Shape<T>::eventHandler()
{
    StateManager::setState(StateManager::HOVERED,
        MouseEvents::isHovered(*this, *this->window));
}
template<typename T>
sf::FloatRect  Shape<T>::getGlobalBounds() const
{
    return T::getGlobalBounds();
}

template<typename T>
void Shape<T>::setPosition(const sf::Vector2f &position)
{
    T::setPosition(position);
}

template<typename T>
sf::Vector2f Shape<T>::getPosition() const
{
    return T::getPosition();
}
#endif
