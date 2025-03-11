//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H
#include "../src/Controller.h"
#include "../src/Graphics/CircleButton.h"
#include "../src/Graphics/Grid.h"
#include "../src/Graphics/RectangleButton.h"


class ButtonController : public Controller{
  sf::Font font = FontManager::DEFAULT;
  RectangleButton r1, r2, r3;
  CircleButton r4, r5, r6;
  Grid grid;
  void addComponents();
  void addEvents();
public:
  ButtonController(Application &);

  void update() override;

  void initialization() override;

  void registerEvents() override;
};



#endif //BUTTONCONTROLLER_H
