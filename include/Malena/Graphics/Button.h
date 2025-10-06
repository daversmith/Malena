#ifndef BUTTON_H
#define BUTTON_H

#pragma once

#include <SFML/Graphics/Text.hpp>

#include "../Interfaces/With.h"
#include "../Managers/FontManager.h"

namespace ml
{
	template<typename T, typename S>
	class Button : public T, public sf::Text //, public virtual UIComponentWith
	{
		static_assert(std::is_base_of_v<UIComponent, T>, "T must be derived from UIComponentWith");

	public:
		Button(const sf::Font &font = FontManager<>::getDefault(),
			   std::optional<S> buttonSize = std::nullopt, const std::string &text = "", unsigned int charSize = 30) :
			 Text(font, text, charSize), T()
		{
			if (buttonSize.has_value())
			{
				*static_cast<T *>(this) = T(buttonSize.value()); // Assign after construction
			}
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}

		void setPosition(const sf::Vector2f &position)
		{
			T::setPosition(position);
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}

		void setString(const sf::String &text)
		{
			sf::Text::setString(text);
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}



		void setTextColor(const sf::Color &color)
		{
			sf::Text::setFillColor(color);
		}

		using T::getPosition;
		using T::setFillColor;

	protected:
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override
		{
			T::draw(target, states);
			sf::Text t = *this;
			target.draw(t, states);
		}
	};
} // namespace ml
#endif // BUTTON_H
