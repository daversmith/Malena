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
        clear();
    }

    void UIComponent::addToApplication(Application& application)
    {
        application_ = &application;
        application.addToApplication(*this);
    }

    void UIComponent::removeComponent(UIComponent* component)
    {
        if (application_)
        {
            application_->removeComponent(component);
        }
    }

    void UIComponent::clear()
    {
        auto it = _children.begin();
        while (it != _children.end()) {
            auto* child = *it;
            removeComponent(child);
            it = _children.erase(it);  // erase() returns next valid iterator
        }

    }
    void *UIComponent::operator new(size_t size)
    {
        UIComponent *obj = static_cast<UIComponent *>(malloc(size));
        obj->isDynamic = true; // Mark as dynamically allocated
        return obj;
    }
} // namespace ml
