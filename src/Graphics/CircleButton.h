//
// Created by Dave R. Smith on 3/9/25.
//

#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#include "Button.h"
#include "Circle.h"



namespace ml {
class CircleButton : public Button<Circle, float>{
  using Button::Button;
};

} // namespace ml

#endif //CIRCLEBUTTON_H


