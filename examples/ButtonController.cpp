//
// Created by Dave R. Smith on 3/10/25.
//

#include "ButtonController.h"
#include "../src/Application/Application.h"

ButtonController::ButtonController(ml::Application &app)
: Controller(app),
  r1(font,sf::Vector2f({50.f, 50.f}), "1"),
  r2(font,sf::Vector2f({50.f, 50.f}), "2"),
  r3(font,sf::Vector2f({50.f, 50.f}), "3"), r4(font, 25.f, "4"),
  r5(font, 25.f, "5"), r6(font, 25.f, "6"), grid({3, 2})
{
}
void ButtonController::addComponents()
{

}

void ButtonController::addEvents()
{
  r1.onClick([this]()
  {
    auto temp = r2.getString();
    r2.setString(r1.getString());
    r1.setString(temp);
  });
  // r1.onUpdate([](){});
}

void ButtonController::update()
{
}

void ButtonController::initialization()
{
  grid.addComponent(r1);
  grid.addComponent(r2);
  grid.addComponent(r3);
  grid.addComponent(r4);
  grid.addComponent(r5);
  grid.addComponent(r6);
  grid.addToApp(*this);
}

void ButtonController::registerEvents()
{
  addEvents();
}
