
#ifndef RECTANGLE_H
#define RECTANGLE_H



#include "Shape.h"

namespace ml {

class Rectangle : public virtual Shape<sf::RectangleShape>
{
    using Shape::Shape;
};


} // namespace ml
#endif //RECTANGLE_H


