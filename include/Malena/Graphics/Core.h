#ifndef SHAPE_H
#define SHAPE_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include <Malena/Interfaces/UIComponent.h>
#include <Malena/Managers/FontManager.h>

namespace ml
{

	template<typename T>
	class Core : public T, public virtual UIComponent
	{
	public:
		using T::T;
		operator const sf::Drawable&() const ;
		static T isItText();

		Core();

		template <typename U>
		explicit Core(const U &obj);

		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		sf::FloatRect getGlobalBounds() const override;

		void setPosition(const sf::Vector2f &position) override;

		sf::Vector2f getPosition() const override;
	};

} // namespace ml

#include "../../../src/Graphics/Core.tpp"
#endif // SHAPE_H
