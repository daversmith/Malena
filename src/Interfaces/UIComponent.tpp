//
// Created by Dave Smith on 1/20/26.
//
#ifndef UICOMPONENT_TPP
#define UICOMPONENT_TPP
#include <Malena/Interfaces/UIComponent.h>

namespace ml
{
    template <typename APP, typename ...Components>
    void UIComponent::addChildren(APP& application, Components&... components)
    {
        application_ = static_cast<Application*>(&application);
        (_children.push_back(&components), ...);  // Fold expression
        (application.addComponent(components), ...);
    }
}
#endif