
#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include "Shape.h"

namespace ml
{
	template<typename Manifest = void>
	class SpriteWith : public virtual Shape<sf::Sprite, Manifest>
	{
	public:
		using Shape<sf::Sprite, Manifest>::Shape;
		// Sprite(const sf::Texture& texture = sf::Texture()) : Shape(texture) {}
	};

	using Sprite = SpriteWith<>;
} // namespace ml
#endif // SPRITE_H
