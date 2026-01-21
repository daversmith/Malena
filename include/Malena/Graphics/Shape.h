#ifndef SHAPE_H
#define SHAPE_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include "../Interfaces/UIComponent.h"
#include "../Managers/FontManager.h"

namespace ml
{

	template<typename T>
	class Shape : public T, public virtual UIComponent
	{
	public:
		using T::T;
		operator const sf::Drawable&() const {
			return static_cast<const T&>(*this);
		}
		static T isItText() {
			if constexpr (std::is_same_v<T, sf::Text>) {
				return sf::Text(FontManager<>::getDefault());
			} else {
				return T();
			}
		}

		Shape() : T(isItText())
		{
		}

		template <typename U>
		explicit Shape(const U &obj) : T(obj)
		{
		}

		void draw(sf::RenderTarget &target, sf::RenderStates states) const override
		{
			target.draw(static_cast<const T>(*this), states);
		}

		sf::FloatRect getGlobalBounds() const override
		{
			return T::getGlobalBounds();
		}

		void setPosition(const sf::Vector2f &position) override
		{
			T::setPosition(position);
		}

		sf::Vector2f getPosition() const override
		{
			return T::getPosition();
		}
	};

} // namespace ml

#endif // SHAPE_H
