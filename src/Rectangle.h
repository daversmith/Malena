//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef RECTANGLE_H
#define RECTANGLE_H


#include <SFML/Graphics.hpp>

#include "Component.h"
#include "StateManager.h"

class Rectangle :
    public sf::RectangleShape,
    public StateManager,
    public Component
{
public:
    using sf::RectangleShape::RectangleShape;

protected:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

public:
    void update() override;

    void eventHandler() override;
    sf::FloatRect getGlobalBounds() const override;
};



#endif //RECTANGLE_H
