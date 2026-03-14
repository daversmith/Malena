//
// Created by Dave R. Smith on 3/5/25.
//

#include <Malena/Managers/UIManager.h>

namespace ml
{
    UIManager::UIManager(const sf::VideoMode &videoMode, const std::string &title, UIController &uiController,
                         sf::RenderWindow &window, Architecture archType) : uiController(&uiController), window(&window)
    {
        window.create(videoMode, title);
        this->window->setFramerateLimit(60);
    }

    void UIManager::draw()
    {
        this->window->clear();
        for (auto &c : ComponentsManager<Core>::getComponents())
        {
           	if (!c->checkFlag(Flag::HIDDEN))
                 window->draw(*dynamic_cast<sf::Drawable*>(c), c->getRenderStates());
        }
        this->window->display();
    }

    void UIManager::fireUpdateEvents()
    {
        EventsManager::fire("update");
    }

    void UIManager::run()
    {
        uiController->initialization();
        uiController->registerEvents();

        while (this->window->isOpen())
        {
            while (const std::optional event = this->window->pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    this->window->close();
                    return;
                }
                fireInputEvents(event);
            }

            fireUpdateEvents();
            draw();
        }
    }

    void UIManager::fireInputEvents(const std::optional<sf::Event> &event)
    {
        if (event->is<sf::Event::MouseButtonPressed>() ||
            event->is<sf::Event::MouseButtonReleased>() ||
            event->is<sf::Event::MouseMoved>())
        {
            EventsManager::fire("draggable", nullptr, nullptr, event);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            EventsManager::fire(
                "click",
                [this, &event](Subscribable &ep) -> bool {
                    auto *c = dynamic_cast<Core*>(&ep);
                    if (!c) return false;

                    bool isClicked = MouseEvents::isClicked(*c, *window);
                    bool isFocused = c->checkFlag(Flag::FOCUSED);

                    if (isClicked)
                    {
                        if (!isFocused)
                        {
                            c->enableFlag(Flag::FOCUSED);
                            EventsManager::fire(
                                "focus",
                                [c](Subscribable &ep2) -> bool {
                                    auto* c2 = dynamic_cast<Core*>(&ep2);
                                    return c2 && c2 == c;
                                },
                                nullptr, event);
                        }
                    }
                    else
                    {
                        if (isFocused)
                        {
                            EventsManager::fire(
                                "blur",
                                [c](Subscribable &ep2) -> bool {
                                    auto* c2 = dynamic_cast<Core*>(&ep2);
                                    return c2 && c2 == c;
                                },
                                nullptr, event);
                            c->disableFlag(Flag::FOCUSED);
                        }
                    }
                    return isClicked;
                },
                nullptr, event);
        }

        if (event->is<sf::Event::MouseMoved>())
        {
            EventsManager::fire(
                "hover",
                [this, &event](Subscribable &ep) -> bool {
                    auto *c = dynamic_cast<Core*>(&ep);
                    if (!c) return false;

                    bool isHovered = MouseEvents::isHovered(*c, *window);
                    if (isHovered)
                        c->enableFlag(ml::Flag::HOVERED);
                    else
                    {
                        c->disableFlag(ml::Flag::HOVERED);
                        EventsManager::fire(
                            "unhover",
                            [](Subscribable &ep2) -> bool {
                                auto* c2 = dynamic_cast<Core*>(&ep2);
                                return c2 && !c2->checkFlag(Flag::HOVERED);
                            },
                            nullptr, event);
                    }
                    return isHovered;
                },
                nullptr, event);
        }
        else if (event->is<sf::Event::TextEntered>())
        {
            EventsManager::fire("textentered", nullptr, nullptr, event);
        }
        else if (event->is<sf::Event::KeyPressed>())
        {
            EventsManager::fire("keypress", nullptr, nullptr, event);
        }
        else if (event->is<sf::Event::MouseWheelScrolled>())
        {
            EventsManager::fire("mousewheel", nullptr, nullptr, event);
        }
    }
} // namespace ml