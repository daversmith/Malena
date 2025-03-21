#ifndef UICOMPONENT_H
#define UICOMPONENT_H


#include <SFML/Graphics.hpp>

#include "../Traits/Messenger.h"
#include "../Managers/WindowManager.h"
#include "Component.h"
#include "../Utilities/MouseEvents.h"
#include <iostream>
#include "../Traits/Positionable.h"
#include "Updateable.h"

namespace ml {

class UIComponent : public virtual  sf::Drawable, public Messenger, public Component, public Stateful,
                    public Updateable, public Positionable
{
protected:
    sf::RenderWindow *window = nullptr;
    bool isDynamic = false;
public:

    explicit UIComponent(sf::RenderWindow &window = WindowManager::getWindow());
    virtual void eventHandler(const std::optional<sf::Event> &event) = 0;
    [[nodiscard]] virtual  sf::FloatRect getGlobalBounds() const = 0;
    virtual ~UIComponent() = default;
    void* operator new(size_t size) {
        UIComponent* obj = static_cast<UIComponent*>(malloc(size));
        obj->isDynamic = true; // Mark as dynamically allocated
        return obj;
    }
    friend class ComponentsManager;
};


} // namespace ml
#endif //UICOMPONENT_H


