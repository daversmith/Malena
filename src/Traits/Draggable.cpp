//
// Created by Dave Smith on 11/3/25.
//

#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Managers/WindowManager.h>
#include <Malena/Traits/MultiCustomFlaggable.h>

namespace ml
{
    void Draggable::handleDragEvent(const std::optional<sf::Event>& event)
    {
        if (!event.has_value()) return;

        auto* positionable = dynamic_cast<Positionable*>(this);
        if (!positionable) return;

        auto* f = dynamic_cast<SingleFlaggable<DraggableManifest::Flags>*>(this);
        if (!f) return;

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
                    f->enableFlag(Flag::DRAGGING);
                    _dragOffset = positionable->getPosition() - mousePos;
                }
            }
        }
        else if (event->is<sf::Event::MouseButtonReleased>())
        {
            auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>();
            if (mouseRelease && mouseRelease->button == sf::Mouse::Button::Left)
                f->disableFlag(Flag::DRAGGING);
        }
        else if (event->is<sf::Event::MouseMoved>())
        {
            if (f->checkFlag(Flag::DRAGGING))
                positionable->setPosition(mousePos + _dragOffset);
        }
    }
}