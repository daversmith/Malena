// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.


#ifndef CONVEX_H
#define CONVEX_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Core.h>
#include <Malena/Traits/Base/Customizable.h>

namespace ml
{

	/**
	 * @brief A framework-integrated convex polygon shape.
	 * @ingroup GraphicsPrimitives
	 *
	 * @c Convex wraps @c sf::ConvexShape inside @c Graphic<>, giving it
	 * the full framework trait set: @c Subscribable (click, hover, update),
	 * @c Flaggable, @c Positionable, and @c Draggable.
	 *
	 * After construction, set the point count and define each vertex using
	 * the inherited @c sf::ConvexShape API. The shape must be convex — all
	 * interior angles less than 180° — for SFML to render it correctly.
	 *
	 * ### Usage
	 * @code
	 * ml::Convex hex;
	 * hex.setPointCount(6);
	 * hex.setPoint(0, {60.f,   0.f});
	 * hex.setPoint(1, {120.f, 30.f});
	 * hex.setPoint(2, {120.f, 90.f});
	 * hex.setPoint(3, {60.f, 120.f});
	 * hex.setPoint(4, {0.f,   90.f});
	 * hex.setPoint(5, {0.f,   30.f});
	 * hex.setFillColor(sf::Color(80, 160, 80));
	 * hex.setPosition({200.f, 200.f});
	 * hex.onClick([]{ std::cout << "hex clicked\n"; });
	 * addComponent(hex);
	 * @endcode
	 *
	 * ### With a manifest
	 * @c ConvexWith<Manifest> layers a manifest's flags and states onto
	 * the shape:
	 * @code
	 * class MyHex : public ml::ConvexWith<MyManifest> {};
	 * @endcode
	 *
	 * @see Graphic, ConvexButton, Circle, Rectangle
	 */
	class MALENA_API Convex : public Graphic<sf::ConvexShape>
	{
	public:
		using Graphic::Graphic;
	};

	/**
	 * @brief @c Convex with an attached manifest.
	 *
	 * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
	 *
	 * @see Convex, Customizable
	 */
	template<typename MANIFEST>
	class ConvexWith : public Convex, public Customizable<MANIFEST> {};
} // namespace ml
#endif // CONVEX_H
