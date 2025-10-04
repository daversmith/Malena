//
// Created by Dave Smith on 10/3/25.
//

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "ResourceManager.h"
#include "SFML/Audio/SoundBuffer.hpp"

namespace ml
{

    template<typename Manifest>
    using SoundManager = ml::ResourceManager<Manifest, sf::SoundBuffer>;
}

#endif //SOUNDMANAGER_H
