
#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include "Shape.h"

namespace ml
{

	class Rectangle: public virtual Shape<sf::RectangleShape>
	{
		using Shape::Shape;
	};

	template <typename MANIFEST>
	class RectangleWith: public virtual With<Rectangle, MANIFEST>{};
} // namespace ml
#endif // RECTANGLE_H
