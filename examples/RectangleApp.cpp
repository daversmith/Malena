//
// Created by Dave R. Smith on 3/5/25.
//

#include "RectangleApp.h"

RectangleApp::RectangleApp()
: Application(sf::VideoMode({420u, 720u}, 32), "Rectangle App", *this),
    square({50.f, 50.f}), square2({50.f, 50.f})
{
    auto &s = *(new Rectangle);
    s.setSize({100, 100});
    s.setPosition({200, 200});
    s.onClick([&s]()
    {
        s.setSize({20.f, 20.f});
    });
    // addComponent(s);
}

void RectangleApp::initialization()
{
    square2.setPosition({100.f, 100.f});
    // addComponent(square);
    // addComponent(square2);
}

void RectangleApp::registerEvents()
{
    square.onClick([this](){
        square2.setSize({60.f, 60.f});
    });
}

void RectangleApp::update()
{
    if(square2.getPosition().x < 200.f && square2.getSize().x >= 60.f)
        square2.move({.05f, 0.f});
}