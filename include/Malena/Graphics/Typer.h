
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Managers/FontManager.h>
#include <Malena/Graphics/Graphic.h>

namespace ml
{
	class Typer : public virtual Graphic<sf::Text>
	{
		void registerEvents();

	public:
		using Graphic::Graphic;
		Typer(const sf::Font &font = FontManager<>::getDefault());
	};

} // namespace ml


#endif // TYPER_H
