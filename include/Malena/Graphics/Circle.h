
#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include <Malena/Graphics/Graphic.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class Circle : public Graphic<sf::CircleShape>
	{
	public:
		using Graphic::Graphic;
	};

	template <typename MANIFEST>
	class CircleWith: public Customizable<Circle, MANIFEST>{};
} // namespace ml
#endif // CIRCLE_H
