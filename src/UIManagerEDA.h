//
// Created by Dave Smith on 3/11/25.
//

#ifndef UIMANAGEREDA_H
#define UIMANAGEREDA_H
#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowManager.h"
#include "AppController.h"
#include "UIComponentAdapter.h"
#include "Manager.h"

class UIManagerEDA : public Manager
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
    //draw
    void draw();
public:
    virtual ~UIManagerEDA() = default;

    UIManagerEDA(const sf::VideoMode& videoMode, const std::string& title, AppController& appLogic,
              sf::RenderWindow &window = WindowManager::window);

    void run();

    void onUpdate(std::function<void()>);
};



#endif //UIMANAGEREDA_H
