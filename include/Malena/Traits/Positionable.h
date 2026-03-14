//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_POSITIONABLE_H
#define PORTFOLIO_POSITIONABLE_H

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <float.h>
#include <queue>
#include <Malena/Utilities/Align.h>
#include <Malena/Traits/Trait.h>


#include "../Utilities/Helper.h"
#include "../Utilities/Tween.h"

namespace ml
{

	class Positionable : public Trait
	{
	private:
		float velocityX{}, velocityY{}, framerate = 60.f;
		sf::Vector2f initialPosition = {FLT_MIN, FLT_MAX};
		sf::Clock clock;
		std::queue<sf::Vector2f> points;
		bool scrolling = false;

		bool error(sf::Vector2f a, sf::Vector2f b, float err = 0.1f);
		void generatePoints(sf::Vector2f position, float duration, Tween tween);
		void generateExponential(sf::Vector2f position, float duration);
		void calcVelocity(sf::Vector2f velocity, float seconds);

	public:
		void setFramerate(float framerate);
		void moveTo(sf::Vector2f position, float seconds = 1.f);
		void moveTo(sf::FloatRect position, float seconds = 1.f);
		void moveDistance(sf::Vector2f distance, float seconds = 1.f);
		bool isScrolling();
		virtual void setPosition(const sf::Vector2f &position) = 0;
		virtual sf::Vector2f getPosition() const = 0;
		// void moveTo(sf::Vector2f position, float duration, Tween tween=LINEAR);

		template<class T>
		void setRightOf(T &obj, float spacing = 0);

		template<class T>
		void setLeftOf(T &obj, float spacing = 0);

		template<class T>
		void setBelow(T &obj, float spacing = 0);

		template<class T>
		void setAbove(T &obj, float spacing = 0);

		template<class T>
		void center(T &obj);

		template<class T>
		void centerHorizonally(T &obj);

		template<class T>
		void centerVertically(T &obj);

		void centerText(sf::Text &obj);

		virtual sf::FloatRect getGlobalBounds() const = 0;
	};

} // namespace ml
#include "../../../src/Traits/Positionable.tpp"
#endif // PORTFOLIO_POSITIONABLE_H