
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
	template <typename MANIFEST>
	class ConvexWith: public With<Convex, MANIFEST>{};
} // namespace ml
#endif // CONVEX_H
