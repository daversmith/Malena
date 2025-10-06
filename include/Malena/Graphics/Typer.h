
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Managers/FontManager.h>
#include <Malena/Graphics/Shape.h>

namespace ml
{
	class Typer : public virtual Shape<sf::Text>
	{
		void registerEvents();

	public:
		using Shape::Shape;
		Typer(const sf::Font &font = FontManager<>::getDefault());
	};

} // namespace ml


#endif // TYPER_H
