
#ifndef CONVEX_H
#define CONVEX_H

#pragma once

#include <Malena/Graphics/Core.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{

	class Convex : public Core<sf::ConvexShape>
	{
	public:
		using Core::Core;
	};

	template <typename MANIFEST>
	class ConvexWith: public Customizable<Convex, MANIFEST>{};
} // namespace ml
#endif // CONVEX_H
