
#ifndef CONVEX_H
#define CONVEX_H

#pragma once

#include "Shape.h"

namespace ml
{

	class Convex : Shape<sf::ConvexShape>
	{
	public:
		using Shape::Shape;
	};
} // namespace ml
#endif // CONVEX_H
