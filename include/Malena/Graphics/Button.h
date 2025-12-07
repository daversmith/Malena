#ifndef BUTTON_H
#define BUTTON_H

#pragma once

#include <SFML/Graphics/Text.hpp>
#include <Malena/Interfaces/UIComponent.h>
#include <Malena/Managers/FontManager.h>
#include <string>

namespace ml
{
	template<typename T, typename S>
	class Button : public T, public sf::Text //, public virtual UIComponentWith
	{
		static_assert(std::is_base_of_v<UIComponent, T>, "T must be derived from UIComponent");

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

		void setPosition(const sf::Vector2f &position) override
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

		void setTextOutlineColor(const sf::Color &color)
		{
			sf::Text::setOutlineColor(color);
		}
		void setTextOutlineThickness(float thickness)
		{
			sf::Text::setOutlineThickness(thickness);
		}

		void setScale(sf::Vector2f scale)
		{
			T::setScale(scale);
			sf::Text::setScale(scale);
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}

		void scale(sf::Vector2f scale)
		{
			T::scale(scale);
			sf::Text::scale(scale);
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}
		void move(sf::Vector2f offset)
		{
			sf::Text::move(offset);
			T::move(offset);
		}
		void rotate(sf::Angle angle)
		{
			sf::Text::rotate(angle);
			T::rotate(angle);
			T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		}
		void setRotation(sf::Angle angle)
		{
			T::setRotation(angle);
			sf::Text::setRotation(angle);
		}
		using T::getPosition;
		using T::setFillColor;
		using T::setOutlineColor;
		using T::setOutlineThickness;
		using T::getGlobalBounds;
		using T::getLocalBounds;
		using T::getOrigin;
		using T::getTransform;
		using T::getInverseTransform;
		using T::setOrigin;
		using T::getGeometricCenter;
		using T::getPoint;
		using T::getPointCount;


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
