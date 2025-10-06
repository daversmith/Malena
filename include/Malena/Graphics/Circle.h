
#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include "Shape.h"

namespace ml
{
	class Circle : public Shape<sf::CircleShape>
	{
	public:
		using Shape::Shape;
	};

	template <typename MANIFEST>
	class CircleWith: public With<Circle, MANIFEST>{};
} // namespace ml
#endif // CIRCLE_H
