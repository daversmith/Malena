
#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#pragma once

#include <Malena/Graphics/Cursor.h>
#include <Malena/Graphics/Sprite.h>
#include <Malena/Graphics/Typer.h>

namespace ml
{
	class TextInput : public ml::Core
	{

		ml::Typer typer;
		ml::Cursor cursor;
	protected:
		void draw(sf::RenderTarget &target, sf::RenderStates states) const;//override;

	public:
		TextInput();
		void setPosition(const sf::Vector2f &position) override;

		sf::Vector2f getPosition() const override;

		[[nodiscard]] sf::FloatRect getGlobalBounds() const override;
		void enableState(Flaggable::State state);
	};

} // namespace ml


#endif // TEXTINPUT_H
