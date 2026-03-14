
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Base/Graphic.h>

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
