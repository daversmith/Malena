//
// Created by Dave Smith on 11/3/25.
//

#include <Malena/Traits/Draggable.h>
namespace ml
{
    bool Draggable::getIsDragging() const
    {
        return isDragging;
    }

    void Draggable::handleDragEvent(const std::optional<sf::Event>& event)
    {

        if (!event.has_value())
        {
            return;
        }
        auto* flaggable = dynamic_cast<Flaggable*>(this);
        if (!flaggable->checkFlag(Flag::DRAGGABLE))
        {
            return;
        }

        auto* positionable = dynamic_cast<Positionable*>(this);
        if (!positionable)
        {
            return;
        }

        sf::Vector2f mousePos = WindowManager::getWindow().mapPixelToCoords(
            sf::Mouse::getPosition(WindowManager::getWindow())
        );

        if (event->is<sf::Event::MouseButtonPressed>())
        {
            auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>();
            if (mousePress && mousePress->button == sf::Mouse::Button::Left)
            {
                if (positionable->getGlobalBounds().contains(mousePos))
                {
                    isDragging = true;
                    dragOffset = positionable->getPosition() - mousePos;
                }
            }
        }
        else if (event->is<sf::Event::MouseButtonReleased>())
        {
            auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>();
            if (mouseRelease && mouseRelease->button == sf::Mouse::Button::Left)
            {
                isDragging = false;
            }
        }
        else if (event->is<sf::Event::MouseMoved>())
        {
            if (isDragging) {
                positionable->setPosition(mousePos + dragOffset);
            }
        }
    }
}
