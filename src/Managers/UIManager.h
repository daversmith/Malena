//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowManager.h"
#include "../Controllers/UIController.h"
#include "../Adapters/UIComponentAdapter.h"
#include "Manager.h"
#include "UIManagerMVC.h"
#include "UIManagerEDA.h"

namespace ml {

class UIManager : public Manager
{
private:
    UIController* uiController;
    sf::RenderWindow *window = nullptr;
public:
    enum Architecture {MVC, EDA, ECS};


    UIManager(const sf::VideoMode& videoMode, const std::string& title, UIController& appLogic,
              sf::RenderWindow &window = WindowManager::window, Architecture architechture = MVC);

private:
    void handleEvents() override;

    void fireEvents(const std::optional<sf::Event> &event) override;

    void updateComponents() override;

    void draw() override;

public:
    void run() override;

    // void onUpdate(std::function<void()>) override;



    virtual ~UIManager() = default;

};


} // namespace ml
#endif //UIMANAGER_H


