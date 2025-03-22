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


#include "../Utilities/Helper.h"
#include "../Utilities/Tween.h"

namespace ml
{

	class Positionable
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

	public:
		void calcVelocity(sf::Vector2f velocity, float seconds);
		void setFramerate(float framerate);
		void moveTo(sf::Vector2f position, float seconds = 1.f);
		void moveTo(sf::FloatRect position, float seconds = 1.f);
		void moveDistance(sf::Vector2f distance, float seconds = 1.f);
		bool isScrolling();
		virtual void setPosition(const sf::Vector2f &position) = 0;
		virtual sf::Vector2f getPosition() const = 0;
		// void moveTo(sf::Vector2f position, float duration, Tween tween=LINEAR);

		template<class T>
		void right(T &obj, float spacing = 0)
		{
			right(*this, obj, spacing);
		}

		template<class T>
		void left(T &obj, float spacing = 0)
		{
			left(*this, obj, spacing);
		}

		template<class T>
		void bottom(T &obj, float spacing = 0)
		{
			bottom(*this, obj, spacing);
		}

		template<class T>
		void top(T &obj, float spacing = 0)
		{
			top(*this, obj, spacing);
		}

		template<class T>
		void center(T &obj)
		{
			center(obj, *this);
		}

		void centerText(sf::Text &obj);

		template<class T>
		static void centerText(const T &obj, sf::Text &text)
		{
			sf::FloatRect textRect = text.getGlobalBounds();
			sf::FloatRect tRect = obj.getGlobalBounds();
			sf::Vector2f center = {textRect.size.x / 2.0f, textRect.size.y / 2.f};
			sf::Vector2f localBounds = {center.x + text.getLocalBounds().position.x,
										center.y + text.getLocalBounds().position.y};
			sf::Vector2f rounded = {std::round(localBounds.x), std::round(localBounds.y)};
			text.setOrigin(rounded);
			text.setPosition({tRect.position.x + tRect.size.x / 2, tRect.position.y + tRect.size.y / 2});
		}

		template<class T, class S>
		static void right(const T &constObj, S &obj, float spacing = 0)
		{
			sf::FloatRect ob = constObj.getGlobalBounds();
			obj.setPosition({ob.position.x + ob.size.x + spacing, ob.position.y});
		}

		template<class T, class S>
		static void left(const T &constObj, S &obj, float spacing = 0)
		{
			sf::FloatRect ob = constObj.getGlobalBounds();
			sf::FloatRect me = obj.getGlobalBounds();
			obj.setPosition({ob.position.x - me.size.x - spacing, ob.position.y});
		}

		template<class T, class S>
		static void bottom(const T &constObj, S &obj, float spacing = 0)
		{
			sf::FloatRect ob = constObj.getGlobalBounds();
			obj.setPosition({ob.position.x, ob.position.y + ob.size.y + spacing});
		}

		template<class T, class S>
		static void top(const T &constObj, S &obj, float spacing = 0)
		{
			sf::FloatRect ob = constObj.getGlobalBounds();
			sf::FloatRect me = obj.getGlobalBounds();
			obj.setPosition({ob.position.x, ob.position.y - me.size.y - spacing});
		}

		template<class T, class S>
		static void center(const T &constObj, S &obj)
		{
			sf::FloatRect ob = constObj.getGlobalBounds();
			sf::FloatRect me = obj.getGlobalBounds();
			obj.setPosition({ob.position.x + ob.size.x / 2.0f - me.size.x / 2.0f,
							 ob.position.y + ob.size.y / 2.0f - me.size.y / 2.0f});
		}

		template<class T, class S>
		static void yCenter(const T &constObj, S &obj)
		{
			float x = obj.getPosition().x;
			center(constObj, obj);
			obj.setPosition({x, obj.getPosition().y});
		}

		template<class T, class S>
		static void xCenter(const T &constObj, S &obj)
		{
			float y = obj.getPosition().y;
			center(constObj, obj);
			obj.setPosition({obj.getPosition().x, y});
		}
		virtual sf::FloatRect getGlobalBounds() const = 0;
	};

} // namespace ml
#endif // PORTFOLIO_POSITIONABLE_H
