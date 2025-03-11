//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIManager.h"
#include "EventsManager.h"

UIManager::UIManager(const sf::VideoMode &videoMode, const std::string &title, AppController& appLogic, sf::RenderWindow &window)
        : window(&window)
{
        this->window->create(videoMode,  title);
        this->window->setFramerateLimit(60);
        UIManager::appLogic = &appLogic;
}

void UIManager::handleEvents()
{
        for(auto &controller : appLogic->getControllers())
        {
                for(auto &c : controller->getUIComponents())
                {
                        c->eventHandler();
                }
        }
}
void UIManager::draw()
{
        this->window->clear();
        for(auto &controller : appLogic->getControllers())
        {
                for(auto &c : controller->getUIComponents())
                {
                        this->window->draw(*c);
                }
        }

        this->window->display();
}

void UIManager::updateComponents()
{

        for(auto &controller : appLogic->getControllers())
        {
                for(auto &c : controller->getUIComponents())
                        c->update();
        }
        EventsManager::fire("update");
        EventsManager::process(appLogic->getUIComponents());
        EventsManager::process(appLogic->getControllers());
}

void UIManager::run()
{
        appLogic->initialization();
        for(auto &controller : appLogic->getControllers())
        {
                controller->initialization();
                controller->registerEvents();
        }
        UIManager::appLogic->addComponent(proxy);
        // proxy.onUpdate([](){});
        while(this->window->isOpen())
        {
                while(const std::optional event = this->window->pollEvent())
                {
                        if(event->is<sf::Event::Closed>())
                        {
                                this->window->close();
                                return;
                        }
                        handleEvents();
                        fireEvents();
                }

                updateComponents();
                for(auto &controller : appLogic->getControllers())
                        controller->update();
                draw();
        }

}


void UIManager::firePublishedEvents()  const
{
        EventsManager::process(appLogic->getControllers());
}

void UIManager::fireEvents()
{
      // if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
      {
              EventsManager::fire("click", [this](UIComponent& c)
              {
                return MouseEvents::isClicked(c, *window);
              });
              firePublishedEvents();
      }
}

void UIManager::onUpdate(std::function<void()> f)
{
        proxy.onUpdate(f);
}
