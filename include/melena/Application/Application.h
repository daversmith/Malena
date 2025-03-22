
//
// Created by Dave Smith on 3/7/25.
//

#include "../Controllers/UIController.h"
#include "../Adapters/UIComponentAdapter.h"
#include "../Managers/UIManager.h"
#ifndef APPLICATION_H
#define APPLICATION_H

namespace ml {

    class Application : public UIManager, public  UIController
    {
    public:
        Application(const sf::VideoMode &videoMode, const std::string &title,
            UIController& appLogic, sf::RenderWindow &window = WindowManager::getWindow());
        // using UIManager::UIManager;



    };
} // namespace ml
#endif //APPLICATION_H


