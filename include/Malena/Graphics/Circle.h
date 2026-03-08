
#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include <Malena/Graphics/Core.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class Circle : public Core<sf::CircleShape>
	{
	public:
		using Core::Core;
	};

	template <typename MANIFEST>
	class CircleWith: public Customizable<Circle, MANIFEST>{};
} // namespace ml
#endif // CIRCLE_H
