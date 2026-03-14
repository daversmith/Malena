#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Customizable.h>

#include <Malena/Graphics/Primitives/RoundedRectangle.h>

namespace ml
{
	class Rectangle : public Graphic<ml::RoundedRectangle>
	{
	public:
		using Graphic::Graphic;
	};

	template <typename MANIFEST>
	class RectangleWith: public Rectangle, public Customizable<MANIFEST>{};
} // namespace ml
#endif // RECTANGLE_H
