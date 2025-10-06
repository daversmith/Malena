
#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include "Shape.h"

namespace ml
{

	template<typename Manifest = void>
	class RectangleWith : public virtual Shape<sf::RectangleShape, Manifest>
	{
		using Shape<sf::RectangleShape, Manifest>::Shape;
	};

	using Rectangle = RectangleWith<>;
} // namespace ml
#endif // RECTANGLE_H
