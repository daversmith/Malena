//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef SHAPE_H
#define SHAPE_H


#include <SFML/Graphics.hpp>

#include "UIComponent.h"
template <typename T>
class Shape :
    public T,
    public UIComponent
{
public:
    using T::T;

protected:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

public:
    void update() override;

    void eventHandler() override;
    // using  T::getGlobalBounds;
    sf::FloatRect  getGlobalBounds() const override;

    void setPosition(const sf::Vector2f &position) override;

    sf::Vector2f getPosition() const override;
};

#include "Shape.cpp"

#endif //SHAPE_H
