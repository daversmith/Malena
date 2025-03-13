//
// Created by Dave R. Smith on 3/5/25.
//

#include "RectangleApp.h"

RectangleApp::RectangleApp()
: Application(sf::VideoMode({420u, 720u}, 32), "Rectangle App", *this),
  button_controller_(*this)
{

}

void RectangleApp::initialization()
{
  addController(button_controller_);
}

void RectangleApp::registerEvents()
{

}

void RectangleApp::update()
{
}