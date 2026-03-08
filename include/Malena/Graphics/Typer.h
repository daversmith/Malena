
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Managers/FontManager.h>
#include <Malena/Graphics/Core.h>

namespace ml
{
	class Typer : public virtual Core<sf::Text>
	{
		void registerEvents();

	public:
		using Core::Core;
		Typer(const sf::Font &font = FontManager<>::getDefault());
	};

} // namespace ml


#endif // TYPER_H
