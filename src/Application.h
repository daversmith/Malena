//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "AppLogic.h"
#include "Component.h"

class Application
{

private:
    // std::vector<Component *> components;
    AppLogic *appLogic;
    //handle events
    void handleEvents();
    void fireEvents();
    void firePublishedEvents() const;
    //update objects
    void updateComponents();

public:
    Application(const sf::VideoMode& videoMode, const std::string& title, AppLogic& appLogic);

    void run();


    //draw
protected:
    void draw();

};



#endif //APPLICATION_H
