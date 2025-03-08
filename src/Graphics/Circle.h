//
// Created by Dave Smith on 3/8/25.
//

#ifndef CIRCLE_H
#define CIRCLE_H
#include "../Shape.h"

class Circle : public Shape<sf::CircleShape>
{
public:
    using Shape::Shape;
};
#endif //CIRCLE_H
