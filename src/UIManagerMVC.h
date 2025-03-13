//
// Created by Dave Smith on 3/11/25.
//

#ifndef UIMANAGERMVC_H
#define UIMANAGERMVC_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowManager.h"
#include "AppController.h"
#include "UIComponentAdapter.h"
#include "Manager.h"
class UIManagerMVC : public Manager
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
    virtual ~UIManagerMVC() = default;

    UIManagerMVC(const sf::VideoMode& videoMode, const std::string& title, AppController& appLogic,
              sf::RenderWindow &window = WindowManager::window);

    void run();

    void onUpdate(std::function<void()>);
};



#endif //UIMANAGERMVC_H
