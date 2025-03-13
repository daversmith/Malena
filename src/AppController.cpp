//
// Created by Dave R. Smith on 3/5/25.
//

#include "AppController.h"

void AppController::addController(Controller &controller)
{
    controllers.push_back(&controller);
}

void AppController::update()
{
    for(auto &c : controllers)
        c->update();
}

void AppController::initialization()
{
    for(auto &c : controllers)
        c->initialization();
}

void AppController::registerEvents()
{
    for(auto &c : controllers)
        c->initialization();
}

const std::vector<Controller *> & AppController::getControllers() const
{
    return controllers;
}
