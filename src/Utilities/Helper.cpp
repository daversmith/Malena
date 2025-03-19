//
// Created by Dave Smith on 11/13/22.
//

#include "Helper.h"

namespace ml {

void Helper::output(sf::Vector2f vector)
{
    std::cout << "x: " << vector.x << ", y: " << vector.y <<std::endl;
}

void Helper::output(sf::FloatRect floatRect)
{
    std::cout << "left: " << floatRect.left << ", top: " << floatRect.top
    << ", width: " << floatRect.width << "height: " << floatRect.height << std::endl;
}
} // namespace ml

