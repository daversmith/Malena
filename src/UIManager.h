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
#include "Manager.h"
#include "UIManagerMVC.h"
#include "UIManagerEDA.h"
class UIManager : public Manager
{
private:
    std::unique_ptr<Manager> uiManager;
public:
    enum Architecture {MVC, EDA, ECS};


    UIManager(const sf::VideoMode& videoMode, const std::string& title, AppController& appLogic,
              sf::RenderWindow &window = WindowManager::window, Architecture architechture = MVC);

private:
    void handleEvents() override;

    void fireEvents() override;

    void firePublishedEvents() const override;

    void updateComponents() override;

    void draw() override;

public:
    void run() override;

    void onUpdate(std::function<void()>) override;



    virtual ~UIManager() = default;

};



#endif //UIMANAGER_H
