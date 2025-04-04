#ifndef CURSOR_H
#define CURSOR_H
#include "../Managers/FontManager.h"
#include "Text.h"

namespace ml
{
	class Cursor : public Text
	{
		inline static sf::Clock clock;
		using Text::setString;
		void registerEvents();

	public:
		using Text::Text;
		Cursor(const sf::Font &font = FontManager::getDefault());
	};
} // namespace ml


#endif // CURSOR_H
