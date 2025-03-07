//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>

#include "MessageManager.h"
#include "Window.h"
#include "MouseEvents.h"
#include <iostream>
class Component : public sf::Drawable, public MessageManager
{
public:
    virtual void update() = 0;
    virtual void eventHandler() = 0;
    virtual  sf::FloatRect getGlobalBounds() const;
};



#endif //COMPONENT_H
