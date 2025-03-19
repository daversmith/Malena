//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIManager.h"
#include "EventsManager.h"

namespace ml {

UIManager::UIManager(const sf::VideoMode &videoMode, const std::string &title, UIController& uiController, sf::RenderWindow &window, Architecture archType )
        : uiController(&uiController), window(&window)
{
        window.create(videoMode, title);
        this->window->setFramerateLimit(60);
}

void UIManager::handleEvents()
{
        for(auto &c : uiController->getUIComponents())
        {
                c->eventHandler();
        }
}
void UIManager::draw()
{
        this->window->clear();
        for(auto &c : uiController->getUIComponents())
        {
                this->window->draw(*c);
        }

        this->window->display();
}

void UIManager::updateComponents()
{

        for(auto &c : uiController->getUIComponents())
                c->update();
        EventsManager::fire("update");
}

void UIManager::run()
{
        uiController->initialization();
        uiController->registerEvents();
        // uiController->addComponent(proxy);
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
                for(auto &c : uiController->getUIComponents())
                        c->update();
                draw();
        }

}



void UIManager::fireEvents(const std::optional<sf::Event> &event)
{
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
                EventsManager::fire("click", [this](UIComponent& c)
                {
                  return MouseEvents::isClicked(c, *window);
                });
        }
        if(event->is<sf::Event::MouseMoved>())
        {
                EventsManager::fire("hover", [this](UIComponent& c)
                {
                  return MouseEvents::isHovered(c, *window);
                });
        }
}

// void UIManager::onUpdate(std::function<void()> f)
// {
//         uiManager->onUpdate(f);
// }
} // namespace ml

