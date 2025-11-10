
#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include <Malena/Graphics/Shape.h>
#include <Malena/Traits/Customizable.h>

#include "RoundedRectangle.h"

namespace ml
{

	class Rectangle: public virtual Shape<ml::RoundedRectangle>
	{
		using Shape::Shape;
	};

	template <typename MANIFEST>
	class RectangleWith: public virtual Customizable<Rectangle, MANIFEST>{};
} // namespace ml
#endif // RECTANGLE_H
