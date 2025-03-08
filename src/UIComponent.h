//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <SFML/Graphics.hpp>

#include "MessageManager.h"
#include "WindowManager.h"
#include "Component.h"
#include "MouseEvents.h"
#include <iostream>
#include "Positionable.h"

#include "Updateable.h"

class UIComponent : public sf::Drawable, public MessageManager, public Component, public StateManager,
                    public Updateable, public Positionable
{
protected:
    sf::RenderWindow *window = nullptr;
    bool isDynamic = false;
public:

    explicit UIComponent(sf::RenderWindow &window = WindowManager::window);
    virtual void eventHandler() = 0;
    [[nodiscard]] virtual  sf::FloatRect getGlobalBounds() const = 0;
    virtual ~UIComponent() = default;
    void* operator new(size_t size) {
        UIComponent* obj = static_cast<UIComponent*>(malloc(size));
        obj->isDynamic = true; // Mark as dynamically allocated
        return obj;
    }
    friend class ComponentsManager;
};



#endif //UICOMPONENT_H
