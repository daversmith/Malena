
#ifndef CONVEX_H
#define CONVEX_H

#pragma once

#include <Malena/Core/Core.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{

	/**
	 * @brief Convex.
	 * @ingroup GraphicsPrimitives
	 */
	class Convex : public Graphic<sf::ConvexShape>
	{
	public:
		using Graphic::Graphic;
	};

	template <typename MANIFEST>
	class ConvexWith: public Convex, public Customizable<MANIFEST>{};
} // namespace ml
#endif // CONVEX_H
