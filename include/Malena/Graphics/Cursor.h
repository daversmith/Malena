#ifndef CURSOR_H
#define CURSOR_H
#include <Malena/Managers/FontManager.h>
#include <Malena/Graphics/Text.h>

namespace ml
{
	class Cursor : public ml::Text
	{
		inline static sf::Clock clock;
		using Text::setString;
		void registerEvents();

	public:
		using ml::Text::Text;
		Cursor(const sf::Font &font = FontManager<>::getDefault());
	};
} // namespace ml


#endif // CURSOR_H
