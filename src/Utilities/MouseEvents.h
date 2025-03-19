//
// Created by Dave R. Smith on 3/3/25.
//

#ifndef MOUSEEVENTS_H
#define MOUSEEVENTS_H

#include <SFML/Graphics.hpp>

namespace ml
{
    class MouseEvents {

    public:
        template <typename T>
        static bool isHovered(const T& obj, const sf::RenderWindow& window);

        static bool isHovered(const sf::FloatRect& bounds,
            const sf::RenderWindow& window);

        template <typename T>
        static bool isClicked(const T& obj, const sf::RenderWindow& window);

        static bool isClicked(const sf::FloatRect& bounds,
            const sf::RenderWindow& window);
    };


}
#include "MouseEvents.cpp"
#endif //MOUSEEVENTS_H


