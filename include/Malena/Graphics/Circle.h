
#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include <Malena/Graphics/Shape.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class Circle : public Shape<sf::CircleShape>
	{
	public:
		using Shape::Shape;
	};

	template <typename MANIFEST>
	class CircleWith: public Customizable<Circle, MANIFEST>{};
} // namespace ml
#endif // CIRCLE_H
