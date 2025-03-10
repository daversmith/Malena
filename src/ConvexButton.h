//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef CONVEXBUTTON_H
#define CONVEXBUTTON_H
#include "Graphics/Button.h"
#include "Graphics/Convex.h"


class ConvexButton : public Button<Convex, std::size_t> {
  using Button::Button;
};


#endif //CONVEXBUTTON_H
