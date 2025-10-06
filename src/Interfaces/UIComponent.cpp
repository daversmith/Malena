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
