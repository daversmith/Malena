//
// Created by Dave R. Smith on 3/5/25.
//

#include "Application.h"

#include "EventsManager.h"

Application::Application(const sf::VideoMode &videoMode, const std::string &title, AppLogic& appLogic)
{
        Window::window.create(videoMode,  title);
        Application::appLogic = &appLogic;
}

void Application::handleEvents()
{
        for(auto & c : appLogic->getComponents())
                c->eventHandler();
}
void Application::draw()
{
        Window::window.clear();
        for(auto &c : appLogic->getComponents())
                Window::window.draw(*c);
        Window::window.display();
}

void Application::updateComponents()
{
        for(auto &c : appLogic->getComponents())
                c->update();
}

void Application::run()
{
        appLogic->initialization();
        appLogic->registerEvents();

        while(Window::window.isOpen())
        {
                while(const std::optional event = Window::window.pollEvent())
                {
                        if(event->is<sf::Event::Closed>())
                        {
                                Window::window.close();
                        }
                        handleEvents();
                }
                fireEvents();
                updateComponents();
                appLogic->update();
                draw();
        }
}


void Application::firePublishedEvents()  const
{
        EventsManager::process(appLogic->getComponents());
}
void Application::fireEvents()
{
      // if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
      {
              EventsManager::fire("click", [](Component& c)
              {
                      return MouseEvents::isClicked(c, Window::window);
              });
              firePublishedEvents();
      }
}
