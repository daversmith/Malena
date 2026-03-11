
#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <Malena/Graphics/Graphic.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class Sprite : public Graphic<sf::Sprite>
	{
	public:
		using Graphic::Graphic;
	};

	template<typename MANIFEST>
	class SpriteWidth: public Sprite, public Customizable<MANIFEST>{};
} // namespace ml
#endif // SPRITE_H
