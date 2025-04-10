
#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include "Shape.h"

namespace ml
{
	class Sprite : public virtual Shape<sf::Sprite>
	{
	public:
		using Shape::Shape;
		// Sprite(const sf::Texture& texture = sf::Texture()) : Shape(texture) {}
	};


} // namespace ml
#endif // SPRITE_H
