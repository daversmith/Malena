#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once


#include <Malena/Graphics/Core.h>
#include <Malena/Traits/Customizable.h>

#include <Malena/Graphics/RoundedRectangle.h>

namespace ml
{
	class Rectangle : public Core<ml::RoundedRectangle>
	{
	public:
		using Core::Core;
	};

	template <typename MANIFEST>
	class RectangleWith: public virtual Customizable<Rectangle, MANIFEST>{};
} // namespace ml
#endif // RECTANGLE_H
