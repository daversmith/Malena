//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONVEX_H
#define CONVEX_H

#include "Shape.h"

namespace ml {

class Convex : Shape<sf::ConvexShape>
{
public:
    using Shape::Shape;
};
} // namespace ml
#endif //CONVEX_H


