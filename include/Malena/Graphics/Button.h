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
	class Button : public T//, public sf::Text //, public virtual UIComponentWith
	{
		static_assert(std::is_base_of_v<UIComponent, T>, "T must be derived from UIComponent");
		sf::Text _text;
	public:
		Button(const sf::Font &font = FontManager<>::getDefault(),
			   std::optional<S> buttonSize = std::nullopt, const std::string &text = "", unsigned int charSize = 30) :
			 _text(font, text, charSize), T()
		{
			if (buttonSize.has_value())
			{
				*static_cast<T *>(this) = T(buttonSize.value()); // Assign after construction
			}
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}

		void setPosition(const sf::Vector2f &position) override
		{
			T::setPosition(position);
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}

		void setString(const sf::String &text)
		{
			_text.setString(text);
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}

		void setCharacterSize(unsigned int size)
		{
			_text.setCharacterSize(size);
		}
		unsigned int getCharacterSize()  const
		{
			return _text.getCharacterSize();
		}

		void setTextColor(const sf::Color &color)
		{
			_text.setFillColor(color);
		}

		void setTextOutlineColor(const sf::Color &color)
		{
			_text.setOutlineColor(color);
		}
		void setTextOutlineThickness(float thickness)
		{
			_text.setOutlineThickness(thickness);
		}

		void setScale(sf::Vector2f scale)
		{
			T::setScale(scale);
			_text.setScale(scale);
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}

		void scale(sf::Vector2f scale)
		{
			T::scale(scale);
			_text.scale(scale);
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}
		void move(sf::Vector2f offset)
		{
			_text.move(offset);
			T::move(offset);
		}
		void rotate(sf::Angle angle)
		{
			_text.rotate(angle);
			T::rotate(angle);
			// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
			T::centerText(*dynamic_cast<T *>(this), _text);
		}
		void setRotation(sf::Angle angle)
		{
			T::setRotation(angle);
			_text.setRotation(angle);
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


	public:
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override
		{
			T::draw(target, states);
			// sf::Text t = *this;
			target.draw(_text, states);
		}
		void setFont(const sf::Font& font)
		{
			_text.setFont(font);
		}
	    void setFont(const sf::Font&& font) = delete;
	    void setLineSpacing(float spacingFactor)
	    {
	    	_text.setLineSpacing(spacingFactor);
	    }
	    void setLetterSpacing(float spacingFactor)
	    {
	    	_text.setLetterSpacing(spacingFactor);
	    }
	    void setStyle(std::uint32_t style)
	    {
	    	_text.setStyle(style);
	    }
	    // void setFillColor(sf::Color color);
	    // void setOutlineColor(sf::Color color);
	    // void setOutlineThickness(float thickness);
	    [[nodiscard]] const sf::String& getString() const
	    {
		    return _text.getString();
	    }
	    [[nodiscard]] const sf::Font& getFont() const
	    {
	    	return _text.getFont();
	    }
	    [[nodiscard]] float getLetterSpacing() const
	    {
	    	return _text.getLetterSpacing();
	    }
	    [[nodiscard]] float getLineSpacing() const
	    {
	    	return _text.getLineSpacing();
	    }
	    [[nodiscard]] std::uint32_t getStyle() const
	    {
	    	return _text.getStyle();
	    }
	    [[nodiscard]] sf::Color getTextColor() const
	    {
	    	return _text.getFillColor();
	    }
	    [[nodiscard]] sf::Color getTextOutlineColor() const
		{
			return _text.getOutlineColor();
		}
	    [[nodiscard]] float getTextOutlineThickness() const
	    {
	    	return _text.getOutlineThickness();
	    }
		[[nodiscard]] sf::Vector2f findCharacterPos(std::size_t index) const
	    {
		    return _text.findCharacterPos(index);
	    }

	};
} // namespace ml
#endif // BUTTON_H
