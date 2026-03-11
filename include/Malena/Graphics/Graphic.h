#ifndef GRAPHIC_H
#define GRAPHIC_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include <Malena/Interfaces/Core.h>
#include <Malena/Managers/FontManager.h>
namespace ml
{

	template<typename T>
	class Graphic : public T, public virtual Core
	{
	public:
		using T::T;
		operator const sf::Drawable&() const ;
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

#include "../../../src/Graphics/Graphic.tpp"
#endif // SHAPE_H
