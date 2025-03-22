
#ifndef TYPER_H
#define TYPER_H
#include "../Managers/FontManager.h"
#include "Shape.h"

namespace ml
{
	class Typer : public virtual ml::Shape<sf::Text>
	{
		void registerEvents();

	public:
		using Shape::Shape;
		Typer(const sf::Font &font = ml::FontManager::DEFAULT);
	};
} // namespace ml


#endif // TYPER_H
