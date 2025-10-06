
#ifndef CONVEX_H
#define CONVEX_H

#pragma once

#include "Shape.h"

namespace ml
{

	template<typename Manifest = void>
	class ConvexWith : Shape<sf::ConvexShape, Manifest>
	{
	public:
		using Shape<sf::ConvexShape, Manifest>::Shape;
	};
	using Convex = ConvexWith<>;
} // namespace ml
#endif // CONVEX_H
