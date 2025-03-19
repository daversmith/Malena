//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H
#include "../src/Controllers/Controller.h"
#include "../src/Graphics/CircleButton.h"
#include "../src/Graphics/Grid.h"
#include "../src/Graphics/RectangleButton.h"


class ButtonController : public ml::Controller{
  sf::Font font = ml::FontManager::DEFAULT;
  ml::RectangleButton r1, r2, r3;
  ml::CircleButton r4, r5, r6;
  ml::Grid grid;
  void addComponents();
  void addEvents();
public:
  ButtonController(ml::Application &);

  void update() override;

  void initialization() override;

  void registerEvents() override;
};



#endif //BUTTONCONTROLLER_H
