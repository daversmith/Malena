#ifndef GRAPHIC_H
#define GRAPHIC_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <Malena/Core/Component.h>
#include <Malena/Resources/FontManager.h>

namespace ml
{
	/**
	 * @brief Base for all Malena graphics.
	 *
	 * Inherits T (an SFML shape/text which provides sf::Drawable)
	 * and ComponentCore<> (Draggable, flags, states, subscriptions)
	 * without adding a second sf::Drawable — no diamond conflict.
	 */
	template<typename T>
	class Graphic : public T, public virtual ComponentCore<>
	{
	public:
		using T::T;
		operator const sf::Drawable&() const;
		static T isItText();

		Graphic();

		template <typename U>
		explicit Graphic(const U &obj);

		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		sf::FloatRect getGlobalBounds() const override;

		void setPosition(const sf::Vector2f &position) override;

		sf::Vector2f getPosition() const override;
	};

} // namespace ml

#include "../../../../src/Graphics/Base/Graphic.tpp"
#endif // SHAPE_H