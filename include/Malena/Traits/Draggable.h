//
// Created by Dave Smith on 11/3/25.
//

#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>

namespace ml
{
    class DraggableManifest : public Manifest
    {
    public:
        enum class State { FREE, LOCK_X, LOCK_Y };
        enum class Flag  { DRAGGING };
    };

    /**
     * @brief Built-in drag trait — always present on every Component.
     *
     * Enable dragging:
     * @code
     * component.enableFlag(ml::Flag::DRAGGABLE);
     * @endcode
     *
     * Axis locking via state:
     * @code
     * component.setState(ml::Draggable::State::LOCK_X);  // Y axis only
     * component.setState(ml::Draggable::State::LOCK_Y);  // X axis only
     * component.setState(ml::Draggable::State::FREE);     // any direction
     * @endcode
     *
     * Drag bounds:
     * @code
     * component.setDragBounds(sf::FloatRect{position, size});
     * component.clearDragBounds();
     * @endcode
     */
    class Draggable : public Customizable<DraggableManifest>
    {
    public:
        using Flag  = DraggableManifest::Flag;
        using State = DraggableManifest::State;

        Draggable() = default;
        virtual ~Draggable() = default;

        void setDragBounds(const sf::FloatRect& bounds);
        void clearDragBounds();

        void handleDragEvent(const std::optional<sf::Event>& event);

    private:
        sf::Vector2f                 _dragOffset;
        std::optional<sf::FloatRect> _dragBounds;
    };

} // namespace ml

#endif //DRAGGABLE_H