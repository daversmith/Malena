//
// Created by Dave Smith on 10/4/25.
//

#ifndef UICOMPONENTBASE_H
#define UICOMPONENTBASE_H


#include <SFML/Graphics.hpp>

#include <Malena/Managers/WindowManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/EventPublisher.h>
#include <Malena/Traits/Flaggable.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Utilities/MouseEvents.h>
#include <iostream>
#include <SFML/Graphics/Sprite.hpp>
#include <list>

namespace ml
    {

        class Application;
        class UIComponent : //public virtual sf::Drawable,
                            // public Messenger, maybe later?
                            public EventPublisher,
                            public Flaggable,
                            public Positionable,
                            public Draggable
        {



            sf::RenderWindow *window = nullptr;
            bool isDynamic = false;
            Application* application_ = nullptr;
            std::list<UIComponent*> _children;
        public:
            explicit UIComponent(sf::RenderWindow &window = WindowManager::getWindow());
            virtual ~UIComponent();
            void *operator new(size_t size);
            void removeComponent(UIComponent* component);
            void clear();
           // void draw(sf::RenderTarget& target, sf::RenderStates states) const override {};
            virtual sf::RenderStates getRenderStates() const { return sf::RenderStates();}
            friend class ComponentsManager;
            friend class Application;
        };



    } // namespace ml

#endif //UICOMPONENTBASE_H
