//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef RECTANGLE_H
#define RECTANGLE_H


#include "../Shape.h"

class Rectangle : public virtual Shape<sf::RectangleShape>
{
    using Shape::Shape;
};



#endif //RECTANGLE_H
