
#ifndef SHAPE_CPP
#define SHAPE_CPP

#include "Shape.h"
#include <iostream>

namespace ml {
    template<typename T>
    Shape<T>::Shape()
    {
    }

    template<typename T>
    Shape<T>::Shape(const sf::Font &font) : sf::Text(font)
    {
    }

    template <typename T>
    void Shape<T>::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        T r = *this;
        target.draw(r);
    }
    template <typename T>
    void Shape<T>::update()
    {
         // if(Stateful::checkState(Stateful::HOVERED))
         //    sf::RectangleShape::setFillColor(sf::Color::Blue);
         // else
         //    sf::RectangleShape::setFillColor(sf::Color::Magenta);
    }
    template <typename T>
    void Shape<T>::eventHandler(const std::optional<sf::Event> &event)
    {
        // Stateful::setState(Stateful::HOVERED,
        //     MouseEvents::isHovered(*this, *this->window));
        // onHover([this]()
        // {
        //     sf::RectangleShape::setFillColor(sf::Color::Blue);;
        // });
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
} // namespace ml

#endif

