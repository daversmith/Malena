//
// Created by Dave R. Smith on 3/9/25.
//

#ifndef RECTANGLEBUTTON_H
#define RECTANGLEBUTTON_H

#include "Rectangle.h"
#include "Button.h"

namespace ml {
class RectangleButton : public  Button<Rectangle, sf::Vector2f>{
public:
  using Button::Button;


};

} // namespace ml

#endif //RECTANGLEBUTTON_H


