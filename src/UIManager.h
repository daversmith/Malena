//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowManager.h"
#include "AppController.h"
#include "UIComponentAdapter.h"

class UIManager
{

private:
    // std::vector<Component *> components;
    AppController *appLogic;
    sf::RenderWindow *window = nullptr;
    //handle events
    void handleEvents();
    void fireEvents();
    void firePublishedEvents() const;
    //update objects
    void updateComponents();
    UIComponentAdapter proxy;

public:
    virtual ~UIManager() = default;

    UIManager(const sf::VideoMode& videoMode, const std::string& title, AppController& appLogic,
              sf::RenderWindow &window = WindowManager::window);

    void run();

    void onUpdate(std::function<void()>);

    //draw
protected:
    void draw();

};



#endif //UIMANAGER_H
