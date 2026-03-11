//
// Created by Dave Smith on 11/3/25.
//

#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#pragma once
#include <SFML/Graphics.hpp>
#include <Malena/Managers/WindowManager.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>

#include "Trait.h"

namespace ml
{
    class DraggableManifest : public Manifest
    {
    public:
        enum class Flags { DRAGGING };
    };

    /**
     * @brief Opt-in trait that gives a component drag behaviour.
     *
     * Inherits Customizable<DraggableManifest> so its flags are
     * automatically gathered into the component's MultiCustomFlaggable.
     *
     * Enable dragging via the system flag:
     * @code
     * _carousel.enableFlag(ml::Flag::DRAGGABLE);
     * @endcode
     *
     * Check drag state via the nested alias:
     * @code
     * _carousel.checkFlag(ml::Draggable::Flag::DRAGGING);
     * @endcode
     */
    class Draggable : public TraitWith<DraggableManifest>
    {
    public:
        using Flag = DraggableManifest::Flags;

        Draggable() = default;
        virtual ~Draggable() = default;

        void handleDragEvent(const std::optional<sf::Event>& event);

    private:
        sf::Vector2f _dragOffset;
    };

} // namespace ml

#endif //DRAGGABLE_H