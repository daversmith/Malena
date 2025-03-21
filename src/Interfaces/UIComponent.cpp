//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIComponent.h"

namespace ml {
    UIComponent::UIComponent(sf::RenderWindow &window) : window(&window)
    {
        onClick([](){});
        onHover([](){});
    }

    UIComponent::~UIComponent()
    {
        unsubscribeAll();
    }
} // namespace ml

