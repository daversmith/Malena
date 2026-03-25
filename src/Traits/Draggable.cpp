//
// Draggable.cpp
//

#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/Event.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Traits/Base/MultiCustomFlaggable.h>
#include <Malena/Traits/Base/MultiCustomStateManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/Flaggable.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Utilities/Flag.h>
#include <algorithm>

namespace ml
{
    // ── Draggable ─────────────────────────────────────────────────────────────

    void Draggable::setDragBounds(const sf::FloatRect& bounds) { _dragBounds = bounds; }
    void Draggable::clearDragBounds()                          { _dragBounds.reset(); }

    // ── DraggableDispatcher ───────────────────────────────────────────────────

    bool DraggableDispatcher::occurred(const std::optional<sf::Event>& event)
    {
        return event.has_value() && (
            event->is<sf::Event::MouseButtonPressed>()  ||
            event->is<sf::Event::MouseButtonReleased>() ||
            event->is<sf::Event::MouseMoved>());
    }

    bool DraggableDispatcher::filter(const std::optional<sf::Event>& event,
                                      Core* component)
    {
        if (!component) return false;
        // Only process components with DRAGGABLE flag set
        return component->checkFlag(ml::Flag::DRAGGABLE);
    }

    void DraggableDispatcher::fire(const std::optional<sf::Event>& event)
    {
        if (!event.has_value()) return;

        EventManager::fire(ml::Event::DRAG, this, event,
            [this](EventReceiver* receiver, const std::optional<sf::Event>& e)
            {
                auto* draggable   = dynamic_cast<Draggable*>(receiver);
                auto* positionable = dynamic_cast<Positionable*>(receiver);

                if (!draggable || !positionable || !e.has_value()) return;

                auto* f = dynamic_cast<SingleFlaggable<DraggableManifest::Flag>*>(receiver);
                auto* s = dynamic_cast<SingleStateManager<DraggableManifest::State>*>(receiver);
                if (!f || !s) return;

                sf::Vector2f mousePos = WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));

                if (e->is<sf::Event::MouseButtonPressed>())
                {
                    auto* press = e->getIf<sf::Event::MouseButtonPressed>();
                    if (press && press->button == sf::Mouse::Button::Left)
                    {
                        if (positionable->getGlobalBounds().contains(mousePos))
                        {
                            f->enableFlag(Draggable::Flag::DRAGGING);
                            draggable->_dragOffset = positionable->getPosition() - mousePos;
                        }
                    }
                }
                else if (e->is<sf::Event::MouseButtonReleased>())
                {
                    auto* release = e->getIf<sf::Event::MouseButtonReleased>();
                    if (release && release->button == sf::Mouse::Button::Left)
                        f->disableFlag(Draggable::Flag::DRAGGING);
                }
                else if (e->is<sf::Event::MouseMoved>())
                {
                    if (!f->checkFlag(Draggable::Flag::DRAGGING)) return;

                    sf::Vector2f currentPos = positionable->getPosition();
                    sf::Vector2f newPos     = mousePos + draggable->_dragOffset;

                    // Apply axis lock
                    switch (s->getState())
                    {
                        case Draggable::State::LOCK_X: newPos.x = currentPos.x; break;
                        case Draggable::State::LOCK_Y: newPos.y = currentPos.y; break;
                        case Draggable::State::FREE:   break;
                    }

                    // Apply drag bounds clamping
                    if (draggable->_dragBounds.has_value())
                    {
                        sf::FloatRect bounds     = positionable->getGlobalBounds();
                        sf::FloatRect dragBounds = draggable->_dragBounds.value();

                        newPos.x = std::clamp(newPos.x,
                            dragBounds.position.x,
                            dragBounds.position.x + dragBounds.size.x - bounds.size.x);

                        newPos.y = std::clamp(newPos.y,
                            dragBounds.position.y,
                            dragBounds.position.y + dragBounds.size.y - bounds.size.y);
                    }

                    positionable->setPosition(newPos);
                }
            });
    }

} // namespace ml
