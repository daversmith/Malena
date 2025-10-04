
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include "../Managers/FontManager.h"
#include "Shape.h"

namespace ml
{
	class Typer : public virtual ml::Shape<sf::Text>
	{
		void registerEvents();

	public:
		using Shape::Shape;
		Typer(const sf::Font &font = ml::FontManager<>::getDefault());
	};
} // namespace ml


#endif // TYPER_H
