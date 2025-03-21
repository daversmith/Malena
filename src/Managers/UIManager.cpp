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

void UIManager::handleEvents(const std::optional<sf::Event> &event)
{
        for(auto &c : uiController->getUIComponents())
        {
                c->eventHandler(event);
        }
}
void UIManager::draw()
{
        this->window->clear();
        for(auto &c : uiController->getUIComponents())
        {
                if(!c->checkState(Stateful::HIDDEN))
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
                std::optional<sf::Event> *evPtr;
                while(const std::optional event = this->window->pollEvent())
                {
                        if(event->is<sf::Event::Closed>())
                        {
                                this->window->close();

                                return;
                        }
                        handleEvents(event);
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
                EventsManager::fire("click",
                    [this, &event](UIComponent& c) -> bool
                    {
                        bool isClicked   = MouseEvents::isClicked(c, *window);
                        bool isFocused   = c.checkState(Stateful::FOCUSED);

                        if(isClicked)
                        {
                            if(!isFocused)
                            {
                                c.enableState(Stateful::FOCUSED);

                                EventsManager::fire("focus",
                                    [this, &c](UIComponent& comp) -> bool
                                    {
                                        return (&comp == &c);
                                    },
                                    nullptr,
                                    event);
                            }
                        }
                        else
                        {
                            if(isFocused)
                            {
                                EventsManager::fire("blur",
                                    [this, &c](UIComponent& comp) -> bool
                                    {
                                        return (&comp == &c);
                                    },
                                    nullptr,
                                    event);
                                c.disableState(Stateful::FOCUSED);
                            }
                        }
                        return isClicked;
                    },
                    nullptr,
                    event);
        }

        /// Checking Event (use else-if)
        if(event->is<sf::Event::MouseMoved>())
        {
                EventsManager::fire("hover", [this, &event](UIComponent& c)
                {
                        bool isHovered = MouseEvents::isHovered(c, *window);
                        if(isHovered)
                                c.enableState(ml::Stateful::HOVERED);
                        else
                        {
                                c.disableState(ml::Stateful::HOVERED);
                                EventsManager::fire("unhover", [&event](UIComponent& c){return !c.checkState(Stateful::HOVERED);}, nullptr, event);
                        }

                        return isHovered;
                }, nullptr, event);
        }
        else if(event->is<sf::Event::TextEntered>())
        {
                EventsManager::fire("textentered", nullptr, nullptr, event);
        }
        else if(event->is<sf::Event::MouseMoved>())
        {
                EventsManager::fire("mousemoved", nullptr, nullptr, event);
        }
        else if(event->is<sf::Event::KeyPressed>())
        {
                EventsManager::fire("keypress", nullptr, nullptr, event);
        }
}

} // namespace ml

