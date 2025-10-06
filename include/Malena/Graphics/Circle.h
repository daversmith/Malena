
#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include "Shape.h"

namespace ml
{
	template<typename Manifest = void>
	class CircleWith : public Shape<sf::CircleShape, Manifest>
	{
	public:
		using Shape<sf::CircleShape, Manifest>::Shape;
	};
	using Circle = CircleWith<>;
} // namespace ml
#endif // CIRCLE_H
