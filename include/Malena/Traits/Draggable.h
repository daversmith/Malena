//
// Created by Dave Smith on 11/3/25.
//

#ifndef DRAGGABLE_H
#define DRAGGABLE_H



#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Malena/Utilities/MouseEvents.h>

#include <Malena/Managers/WindowManager.h>

#include "Flaggable.h"
#include "Positionable.h"

namespace ml {

    class Draggable {
    private:
        bool isDragging = false;
        sf::Vector2f dragOffset;

    public:
        virtual ~Draggable() = default;

        [[nodiscard]] bool getIsDragging() const;

        void handleDragEvent(const std::optional<sf::Event>& event);
    };

} // namespace ml

#endif //DRAGGABLE_H
