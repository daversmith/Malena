
#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <Malena/Graphics/Shape.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	using Sprite = Shape<sf::Sprite>;
	template<typename MANIFEST>
	using SpriteWidth =  Customizable<Sprite, MANIFEST>;
	// class Sprite : public virtual Shape<sf::Sprite>
	// {
	// public:
	// 	using Shape::Shape;
	// 	// Sprite(const sf::Texture& texture = sf::Texture()) : Shape(texture) {}
	// };
	//
	// template<typename MANIFEST>
	// class SpriteWidth: public Customizable<Sprite, MANIFEST>{};
} // namespace ml
#endif // SPRITE_H
