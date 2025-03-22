//
// Created by Dave Smith on 3/11/25.
//

#ifndef RENDERINGSYSTEM_H
#define RENDERINGSYSTEM_H

#include <SFML/Graphics.hpp>
#include "ECSManager.h"

class RenderingSystem {
public:
    void render(sf::RenderWindow& window, ECSManager& ecs) {
        int i = 1;
        for (auto& [entity, position] : ecs.positions) {
            sf::CircleShape shape(ecs.size[i++].health);
            shape.setPosition({position.x, position.y});
            shape.setFillColor(sf::Color(rand()%256, rand()%256, rand()%256));
            window.draw(shape);
        }
    }
};



#endif //RENDERINGSYSTEM_H
