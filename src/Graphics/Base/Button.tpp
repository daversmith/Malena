//
// Created by Dave Smith on 3/6/26.
//
#ifndef BUTTON_CPP
#define BUTTON_CPP

#pragma once

#include <SFML/Graphics/Text.hpp>
#include <Malena/Core/Core.h>
#include <Malena/Resources/FontManager.h>
#include <string>

namespace ml
{
	template<typename T, typename S>
	Button<T, S>::Button(const sf::Font &font, std::optional<S> buttonSize, const std::string &text, unsigned int charSize) :
			 _text(font, text, charSize), T()
	{
		if (buttonSize.has_value())
		{
			*static_cast<T *>(this) = T(buttonSize.value()); // Assign after construction
		}
		// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		T::centerText( _text);
	}

	template<typename T, typename S>
	void Button<T, S>::setPosition(const sf::Vector2f &position)
	{
		T::setPosition(position);
		// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		T::centerText( _text);
	}

	template<typename T, typename S>
	void Button<T, S>::setString(const sf::String &text)
	{
		_text.setString(text);
		// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		T::centerText( _text);
	}

	template<typename T, typename S>
	void Button<T, S>::setCharacterSize(unsigned int size)
	{
		_text.setCharacterSize(size);
	}

	template<typename T, typename S>
	unsigned int Button<T, S>::getCharacterSize()  const
	{
		return _text.getCharacterSize();
	}

	template<typename T, typename S>
	void Button<T, S>::setTextColor(const sf::Color &color)
	{
		_text.setFillColor(color);
	}

	template<typename T, typename S>
	void Button<T, S>::setTextOutlineColor(const sf::Color &color)
	{
		_text.setOutlineColor(color);
	}

	template<typename T, typename S>
	void Button<T, S>::setTextOutlineThickness(float thickness)
	{
		_text.setOutlineThickness(thickness);
	}

	template<typename T, typename S>
	void Button<T, S>::setScale(sf::Vector2f scale)
	{
		T::setScale(scale);
		_text.setScale(scale);
		T::centerText(_text);
	}

	template<typename T, typename S>
	void Button<T, S>::scale(sf::Vector2f scale)
	{
		T::scale(scale);
		_text.scale(scale);
		// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		T::centerText( _text);
	}

	template<typename T, typename S>
	void Button<T, S>::move(sf::Vector2f offset)
	{
		_text.move(offset);
		T::move(offset);
	}

	template<typename T, typename S>
	void Button<T, S>::rotate(sf::Angle angle)
	{
		_text.rotate(angle);
		T::rotate(angle);
		// T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
		T::centerText( _text);
	}

	template<typename T, typename S>
	void Button<T, S>::setRotation(sf::Angle angle)
	{
		T::setRotation(angle);
		_text.setRotation(angle);
	}

	template<typename T, typename S>
	void Button<T, S>::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		T::draw(target, states);
		// sf::Text t = *this;
		target.draw(_text, states);
	}

	template<typename T, typename S>
	void Button<T, S>::setFont(const sf::Font& font)
	{
		_text.setFont(font);
	}

	template<typename T, typename S>
    void Button<T, S>::setLineSpacing(float spacingFactor)
    {
	    _text.setLineSpacing(spacingFactor);
    }

	template<typename T, typename S>
    void Button<T, S>::setLetterSpacing(float spacingFactor)
    {
	    _text.setLetterSpacing(spacingFactor);
    }

	template<typename T, typename S>
    void Button<T, S>::setStyle(std::uint32_t style)
    {
	    _text.setStyle(style);
    }

	template<typename T, typename S>
    [[nodiscard]] const sf::String& Button<T, S>::getString() const
    {
	    return _text.getString();
    }

	template<typename T, typename S>
    [[nodiscard]] const sf::Font& Button<T, S>::getFont() const
    {
	    return _text.getFont();
    }

	template<typename T, typename S>
    [[nodiscard]] float Button<T, S>::getLetterSpacing() const
    {
	    return _text.getLetterSpacing();
    }

	template<typename T, typename S>
    [[nodiscard]] float Button<T, S>::getLineSpacing() const
    {
	    return _text.getLineSpacing();
    }

	template<typename T, typename S>
    [[nodiscard]] std::uint32_t Button<T, S>::getStyle() const
    {
	    return _text.getStyle();
    }

	template<typename T, typename S>
    [[nodiscard]] sf::Color Button<T, S>::getTextColor() const
    {
	    return _text.getFillColor();
    }

	template<typename T, typename S>
    [[nodiscard]] sf::Color Button<T, S>::getTextOutlineColor() const
	{
		return _text.getOutlineColor();
	}

	template<typename T, typename S>
    [[nodiscard]] float Button<T, S>::getTextOutlineThickness() const
    {
	    return _text.getOutlineThickness();
    }

	template<typename T, typename S>
	[[nodiscard]] sf::Vector2f Button<T, S>::findCharacterPos(std::size_t index) const
    {
	    return _text.findCharacterPos(index);
    }
} // namespace ml
#endif // BUTTON_CPP
