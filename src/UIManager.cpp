//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIManager.h"
#include "EventsManager.h"

UIManager::UIManager(const sf::VideoMode &videoMode, const std::string &title, AppController& appLogic, sf::RenderWindow &window, Architecture archType )
{

        if (archType == MVC) {
                // uiManager = std::make_unique<UIManagerMVC>(videoMode,title, appLogic, window);
                uiManager = std::make_unique<UIManagerMVC>(videoMode,title, appLogic);
        } else {
                uiManager = std::make_unique<UIManagerEDA>(videoMode,title, appLogic);
        }
}

void UIManager::handleEvents()
{
        uiManager->handleEvents();
}
void UIManager::draw()
{
        uiManager->draw();
}

void UIManager::updateComponents()
{

       uiManager->updateComponents();
}

void UIManager::run()
{
        uiManager->run();

}


void UIManager::firePublishedEvents()  const
{
       uiManager->firePublishedEvents();
}

void UIManager::fireEvents()
{
      uiManager->fireEvents();
}

void UIManager::onUpdate(std::function<void()> f)
{
        uiManager->onUpdate(f);
}
