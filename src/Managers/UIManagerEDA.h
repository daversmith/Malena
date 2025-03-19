//
// Created by Dave Smith on 3/11/25.
//

#ifndef UIMANAGEREDA_H
#define UIMANAGEREDA_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowManager.h"
#include "../Controllers/UIController.h"
#include "../Adapters/UIComponentAdapter.h"
#include "Manager.h"

namespace ml {

class UIManagerEDA : public Manager
{
private:
    // std::vector<Component *> components;
    UIController *appLogic;
    sf::RenderWindow *window = nullptr;
    //handle events
    void handleEvents();
    void fireEvents(const std::optional<sf::Event> &event);
    //update objects
    void updateComponents();
    UIComponentAdapter proxy;
    //draw
    void draw();
public:
    virtual ~UIManagerEDA() = default;

    UIManagerEDA(const sf::VideoMode& videoMode, const std::string& title, UIController& appLogic,
              sf::RenderWindow &window = WindowManager::window);

    void run();

    // void onUpdate(std::function<void()>);
};

} // namespace ml

#endif //UIMANAGEREDA_H


