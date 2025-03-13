//
// Created by Dave Smith on 3/12/25.
//

#ifndef SPRITE_H
#define SPRITE_H


#include "../Shape.h"
class Sprite : public virtual Shape<sf::Sprite>
{
    using Shape::Shape;
};



#endif //SPRITE_H
