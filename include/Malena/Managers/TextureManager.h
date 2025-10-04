//
// Created by Dave Smith on 4/8/25.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SFML/Graphics/Texture.hpp>

#include "ResourceManager.h"

namespace ml
{

    template<typename Manifest>
    using TextureManager = ml::ResourceManager<Manifest, sf::Texture>;
}


#endif //TEXTUREMANAGER_H
