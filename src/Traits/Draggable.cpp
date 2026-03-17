//
// Created by Dave Smith on 11/3/25.
//

#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <algorithm>


namespace ml
{
    void Draggable::setDragBounds(const sf::FloatRect& bounds) { _dragBounds = bounds; }
    void Draggable::clearDragBounds()                          { _dragBounds.reset(); }

    void Draggable::handleDragEvent(const std::optional<sf::Event>& event)
    {
        if (!event.has_value()) return;

        auto* positionable = dynamic_cast<Positionable*>(this);
        if (!positionable) return;

        auto* f = dynamic_cast<SingleFlaggable<DraggableManifest::Flag>*>(this);
        if (!f) return;

        auto* s = dynamic_cast<SingleStateManager<DraggableManifest::State>*>(this);
        if (!s) return;

        sf::Vector2f mousePos = WindowManager::getWindow().mapPixelToCoords(
            sf::Mouse::getPosition(WindowManager::getWindow())
        );

        if (event->is<sf::Event::MouseButtonPressed>())
        {
            auto* press = event->getIf<sf::Event::MouseButtonPressed>();
            if (press && press->button == sf::Mouse::Button::Left)
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
            auto* release = event->getIf<sf::Event::MouseButtonReleased>();
            if (release && release->button == sf::Mouse::Button::Left)
                f->disableFlag(Flag::DRAGGING);
        }
        else if (event->is<sf::Event::MouseMoved>())
        {
            if (!f->checkFlag(Flag::DRAGGING)) return;

            sf::Vector2f currentPos = positionable->getPosition();
            sf::Vector2f newPos     = mousePos + _dragOffset;

            // Apply axis lock via state
            switch (s->getState())
            {
                case State::LOCK_X: newPos.x = currentPos.x; break;
                case State::LOCK_Y: newPos.y = currentPos.y; break;
                case State::FREE:   break;
            }

            // Apply drag bounds clamping
            if (_dragBounds.has_value())
            {
                sf::FloatRect bounds     = positionable->getGlobalBounds();
                sf::FloatRect dragBounds = _dragBounds.value();

                newPos.x = std::clamp(newPos.x,
                    dragBounds.position.x,
                    dragBounds.position.x + dragBounds.size.x - bounds.size.x);

                newPos.y = std::clamp(newPos.y,
                    dragBounds.position.y,
                    dragBounds.position.y + dragBounds.size.y - bounds.size.y);
            }

            positionable->setPosition(newPos);
        }
    }
}