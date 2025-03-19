//
// Created by Dave Smith on 3/8/25.
//

#ifndef CIRCLE_H
#define CIRCLE_H

#include "Shape.h"

namespace ml {

class Circle : public Shape<sf::CircleShape>
{
public:
    using Shape::Shape;
};
} // namespace ml
#endif //CIRCLE_H


