#ifndef SHAPE_H
#define SHAPE_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include "../Interfaces/UIComponentWith.h"
#include "../Managers/FontManager.h"

namespace ml
{

	template<typename T, typename Manifest = void>
	class Shape : public T, public virtual UIComponentWith<Manifest>
	{
	public:
		using T::T;
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

		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
		{
			T r = *this;
			target.draw(r);
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
