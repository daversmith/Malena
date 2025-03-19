//
// Created by Dave R. Smith on 3/3/25.
//


#ifndef MOUSEEVENTS_CPP
#define MOUSEEVENTS_CPP
#include "MouseEvents.h"

namespace ml
{
    template <typename T>
    bool MouseEvents::isHovered(const T& obj, const sf::RenderWindow& window)
    {
        return isHovered(obj.getGlobalBounds(), window);
    }

    template <typename T>
    bool MouseEvents::isClicked(const T& obj, const sf::RenderWindow& window)
    {
        return isClicked(obj.getGlobalBounds(), window);
    }
}

#endif

