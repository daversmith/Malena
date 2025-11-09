//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Interfaces/UIComponent.h>

#include <Malena/Application/Application.h>

namespace ml
{
    UIComponent::UIComponent(sf::RenderWindow &window) : window(&window)
    {
        onClick([]() {});
        onHover([]() {});
        subscribe("draggable", [this](const std::optional<sf::Event>& event)
        {
            if (checkFlag(Flag::DRAGGABLE))
                this->handleDragEvent(event);
        });
    }

    UIComponent::~UIComponent()
    {
        unsubscribeAll();
    }

    void UIComponent::addToApplication(Application& application)
    {
        application.addToApplication(*this);
    }
} // namespace ml
