//
// Created by Dave R. Smith on 3/9/25.
//

#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H
#include "Button.h"
#include "Circle.h"


class CircleButton : public Button<Circle, float>{
  using Button::Button;
};



#endif //CIRCLEBUTTON_H
