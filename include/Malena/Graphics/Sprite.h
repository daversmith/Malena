
#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <Malena/Graphics/Core.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class Sprite : public Core<sf::Sprite>
	{
	public:
		using Core::Core;
	};

	template<typename MANIFEST>
	class SpriteWidth: public Customizable<Sprite, MANIFEST>{};
} // namespace ml
#endif // SPRITE_H
