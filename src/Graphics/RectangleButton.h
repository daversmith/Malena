//
// Created by Dave R. Smith on 3/9/25.
//

#ifndef RECTANGLEBUTTON_H
#define RECTANGLEBUTTON_H
#include "Button.h"

class RectangleButton : public  Button<Rectangle, sf::Vector2f>{
public:
  using Button::Button;


};



#endif //RECTANGLEBUTTON_H
