//
// Created by Dave Smith on 10/4/25.
//

#ifndef UICOMPONENTBASE_H
#define UICOMPONENTBASE_H


#include <SFML/Graphics.hpp>

#include <Malena/Managers/WindowManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/Subscribable.h>
#include <Malena/Traits/Flaggable.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Utilities/MouseEvents.h>
#include <iostream>
#include <SFML/Graphics/Sprite.hpp>
#include <list>

namespace ml
    {

        class Core : public virtual Subscribable,
                     public virtual Flaggable,
                     public virtual Positionable//,
                    // public virtual Draggable
        {
        public:
            virtual ~Core();
            virtual sf::RenderStates getRenderStates() const { return sf::RenderStates();}
        };
    } // namespace ml

#endif //UICOMPONENTBASE_H
