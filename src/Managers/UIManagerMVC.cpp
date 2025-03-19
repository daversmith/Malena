//
// Created by Dave Smith on 3/11/25.
//

#include "UIManagerMVC.h"
//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIManagerMVC.h"
#include "EventsManager.h"

namespace ml {

UIManagerMVC::UIManagerMVC(const sf::VideoMode &videoMode, const std::string &title, UIController& appLogic, sf::RenderWindow &window)
        : window(&window)
{

        this->window->create(videoMode,  title);
        this->window->setFramerateLimit(60);
        UIManagerMVC::appLogic = &appLogic;
}

void UIManagerMVC::handleEvents()
{
        for(auto &controller : appLogic->getControllers())
        {
                for(auto &c : controller->getUIComponents())
                {
                        c->eventHandler();
                }
        }
}
void UIManagerMVC::draw()
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

void UIManagerMVC::updateComponents()
{

        for(auto &controller : appLogic->getControllers())
        {
                for(auto &c : controller->getUIComponents())
                        c->update();
        }
        EventsManager::fire("update");
}

void UIManagerMVC::run()
{
        appLogic->initialization();

        for(auto &controller : appLogic->getControllers())
        {
                controller->initialization();
                controller->registerEvents();
        }
        UIManagerMVC::appLogic->addComponent(proxy);
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
                        fireEvents(event);
                }

                updateComponents();
                for(auto &controller : appLogic->getControllers())
                        controller->update();
                draw();
        }

}

void UIManagerMVC::fireEvents(const std::optional<sf::Event> &event)
{
      // if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
      {
              EventsManager::fire("click", [this](UIComponent& c)
              {
                return MouseEvents::isClicked(c, *window);
              });
      }
        EventsManager::fire("hover", [this](UIComponent& c)
             {
               return MouseEvents::isHovered(c, *window);
             });
}

// void UIManagerMVC::onUpdate(std::function<void()> f)
// {
//         proxy.onUpdate(f);
// }
} // namespace ml

