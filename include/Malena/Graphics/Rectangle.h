
#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include <Malena/Graphics/Shape.h>
#include <Malena/Traits/Customizable.h>
namespace ml
{

	class Rectangle: public virtual Shape<sf::RectangleShape>
	{
		using Shape::Shape;
	};

	template <typename MANIFEST>
	class RectangleWith: public virtual Customizable<Rectangle, MANIFEST>{};
} // namespace ml
#endif // RECTANGLE_H
