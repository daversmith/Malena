
#ifndef TEXT_H
#define TEXT_H

#pragma once

#include "../Managers/FontManager.h"
#include "Shape.h"

namespace ml
{
	template<typename Manifest = void>
	class TextWith : public virtual Shape<sf::Text, Manifest>
	{
	public:
		void setWordWrap(bool word_wrap);

		void setMaxWidth(float max_width);

	private:
		bool wordWrap = false;
		float maxWidth = 0.f;

	public:
		using Shape<sf::Text, Manifest>::Shape;
		explicit TextWith(const sf::Font &font = FontManager<>::getDefault());
		void setString(const sf::String &text);
	};

	using Text = TextWith<>;


} // namespace ml
#include "../../../src/Graphics/Text.cpp"
#endif // TEXT_H
