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
#include "Component.h"

namespace ml
    {

        class Application;
        class UIComponent : public virtual sf::Drawable,
                            public Messenger,
                            public EventPublisher,
                            public Component,
                            public Flaggable,
                            public Positionable,
                            public Draggable
        {
            /**
         * @brief Override if Custom UIComponent consists of multiple UIComponents
         *
         * @details This function should simply call application.addToApplication(<UiComponent>) for each UIComponent
         *          Member variable in the child class
         * @return nothing is returned
         */
            virtual void addToApplication(Application& application);


            sf::RenderWindow *window = nullptr;
            bool isDynamic = false;

        public:
            explicit UIComponent(sf::RenderWindow &window = WindowManager::getWindow());
            virtual ~UIComponent();
            void *operator new(size_t size)
            {
                UIComponent *obj = static_cast<UIComponent *>(malloc(size));
                obj->isDynamic = true; // Mark as dynamically allocated
                return obj;
            }

            friend class ComponentsManager;
            friend class Application;
        };



    } // namespace ml


#endif //UICOMPONENTBASE_H
