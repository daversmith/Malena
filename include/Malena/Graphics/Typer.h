
#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Managers/FontManager.h>
#include <Malena/Graphics/Shape.h>

namespace ml
{
	template<typename Manifest = void >
	class TyperWith : public virtual Shape<sf::Text, Manifest>
	{
		void registerEvents();

	public:
		using Shape<sf::Text, Manifest>::Shape;
		TyperWith(const sf::Font &font = FontManager<>::getDefault());
	};

	using Typer = TyperWith<>;

} // namespace ml
#include "../../../src/Graphics/Typer.cpp"

#endif // TYPER_H
