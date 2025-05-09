
#ifndef TEXT_H
#define TEXT_H

#pragma once

#include "../Managers/FontManager.h"
#include "Shape.h"

namespace ml
{

	class Text : public virtual Shape<sf::Text>
	{
	public:
		void setWordWrap(bool word_wrap);

		void setMaxWidth(float max_width);

	private:
		bool wordWrap = false;
		float maxWidth = 0.f;

	public:
		using Shape::Shape;
		explicit Text(const sf::Font &font = FontManager::getDefault());
		void setString(const sf::String &text);
	};


} // namespace ml
#endif // TEXT_H
