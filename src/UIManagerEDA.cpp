//
// Created by Dave Smith on 3/11/25.
//

#include "UIManagerEDA.h"
//
// Created by Dave Smith on 3/11/25.
//

#include "UIManagerMVC.h"
//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIManagerMVC.h"
#include "EventsManager.h"

UIManagerEDA::UIManagerEDA(const sf::VideoMode &videoMode, const std::string &title, AppController& appLogic, sf::RenderWindow &window)
        : window(&window)
{
        this->window->create(videoMode,  title);
        this->window->setFramerateLimit(60);
        UIManagerEDA::appLogic = &appLogic;
}

void UIManagerEDA::handleEvents()
{
        for(auto &c : appLogic->getUIComponents())
        {
                c->eventHandler();
        }
}
void UIManagerEDA::draw()
{
        this->window->clear();
        for(auto &c : appLogic->getUIComponents())
        {
                this->window->draw(*c);
        }

        this->window->display();
}

void UIManagerEDA::updateComponents()
{

        for(auto &c : appLogic->getUIComponents())
                c->update();
        EventsManager::fire("update");
        EventsManager::process(appLogic->getUIComponents());
}

void UIManagerEDA::run()
{
        appLogic->initialization();
        appLogic->registerEvents();
        UIManagerEDA::appLogic->addComponent(proxy);
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
                for(auto &c : appLogic->getUIComponents())
                        c->update();
                draw();
        }

}


void UIManagerEDA::firePublishedEvents()  const
{
        EventsManager::process(appLogic->getControllers());
}

void UIManagerEDA::fireEvents()
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

void UIManagerEDA::onUpdate(std::function<void()> f)
{
        proxy.onUpdate(f);
}
