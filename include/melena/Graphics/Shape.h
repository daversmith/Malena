#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics/RenderTarget.hpp>

#include "../Interfaces/UIComponent.h"

namespace ml
{

	template<typename T>
	class Shape : public T, public UIComponent
	{
	public:
		using T::T;

		Shape()
		{
		}

		explicit Shape(const sf::Font &font) : sf::Text(font)
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
