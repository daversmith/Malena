#ifndef BUTTON_H
#define BUTTON_H

#pragma once

#include <SFML/Graphics/Text.hpp>
#include <Malena/Core/Core.h>
#include <Malena/Resources/FontManager.h>
#include <string>

namespace ml
{
	template<typename T, typename S>
	class Button : public T//, public sf::Text //, public virtual UIComponentWith
	{
		static_assert(std::is_base_of_v<Core, T>, "T must be derived from UIComponent");
		sf::Text _text;
	public:
		Button(const sf::Font &font = FontManager<>::getDefault(),
			   std::optional<S> buttonSize = std::nullopt, const std::string &text = "", unsigned int charSize = 30);
		void setPosition(const sf::Vector2f &position) override;
		void setString(const sf::String &text);
		void setCharacterSize(unsigned int size);
		unsigned int getCharacterSize()  const;
		void setTextColor(const sf::Color &color);
		void setTextOutlineColor(const sf::Color &color);
		void setTextOutlineThickness(float thickness);
		void setScale(sf::Vector2f scale);
		void scale(sf::Vector2f scale);
		void move(sf::Vector2f offset);
		void rotate(sf::Angle angle);
		void setRotation(sf::Angle angle);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
		void setFont(const sf::Font& font);
	    void setFont(const sf::Font&& font) = delete;
	    void setLineSpacing(float spacingFactor);
	    void setLetterSpacing(float spacingFactor);
	    void setStyle(std::uint32_t style);
	    [[nodiscard]] const sf::String& getString() const;
	    [[nodiscard]] const sf::Font& getFont() const;
	    [[nodiscard]] float getLetterSpacing() const;
	    [[nodiscard]] float getLineSpacing() const;
	    [[nodiscard]] std::uint32_t getStyle() const;
	    [[nodiscard]] sf::Color getTextColor() const;
	    [[nodiscard]] sf::Color getTextOutlineColor() const;
	    [[nodiscard]] float getTextOutlineThickness() const;
		[[nodiscard]] sf::Vector2f findCharacterPos(std::size_t index) const;

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

	};
} // namespace ml
#include "../../../../src/Graphics/Base/Button.tpp"
#endif // BUTTON_H
