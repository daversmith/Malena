
#ifndef RECTANGLEBUTTON_H
#define RECTANGLEBUTTON_H

#include "Rectangle.h"
#include "Button.h"

namespace ml {
class RectangleButton : public  Button<Rectangle, sf::Vector2f>{
public:
  using Button::Button;
  using sf::RectangleShape::setFillColor;


};

} // namespace ml

#endif //RECTANGLEBUTTON_H


