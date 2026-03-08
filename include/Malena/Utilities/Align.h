//
// Created by Dave Smith on 11/13/22.
//

#ifndef ALIGN_H
#define ALIGN_H

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <float.h>
#include <queue>


#include "../Utilities/Helper.h"
#include "../Utilities/Tween.h"

namespace ml
{

	class Align
	{
	public:

		template<class T>
		static void centerText(const T &obj, sf::Text &text);

		template<class T, class S>
		static void setRightOf(const T &constObj, S &obj, float spacing = 0);

		template<class T, class S>
		static void setLeftOf(const T &constObj, S &obj, float spacing = 0);

		template<class T, class S>
		static void setBelow(const T &constObj, S &obj, float spacing = 0);

		template<class T, class S>
		static void setAbove(const T &constObj, S &obj, float spacing = 0);

		template<class T, class S>
		static void centerOn(const T &constObj, S &obj);

		template<class T, class S>
		static void centerVertically(const T &constObj, S &obj);

		template<class T, class S>
		static void centerHorizontally(const T &constObj, S &obj);

		template<class S>
		static void setRightOf(const sf::FloatRect &bounds, S &obj, float spacing = 0);

		template<class S>
		static void setLeftOf(const sf::FloatRect &bounds, S &obj, float spacing = 0);

		template<class S>
		static void setBelow(const sf::FloatRect &bounds, S &obj, float spacing = 0);

		template<class S>
		static void setAbove(const sf::FloatRect &bounds, S &obj, float spacing = 0);

		template<class S>
		static void centerOn(const sf::FloatRect &bounds, S &obj);

		template<class S>
		static void centerVertically(const sf::FloatRect &bounds, S &obj);

		template<class S>
		static void centerHorizontally(const sf::FloatRect &bounds, S &obj);

		template< class S, class U>
		static void setRightOf(const sf::Vector2<U> &size, S &obj, float spacing = 0);

		template< class S, class U>
		static void setLeftOf(const sf::Vector2<U> &size, S &obj, float spacing = 0);

		template<class S, class U>
		static void setBelow(const sf::Vector2<U> &size, S &obj, float spacing = 0);

		template<class S, class U>
		static void setAbove(const sf::Vector2<U> &size, S &obj, float spacing = 0);

		template<class S, class U>
		static void centerOn(const sf::Vector2<U> &size, S &obj);

		template<class S, class U>
		static void centerVertically(const sf::Vector2<U> &size, S &obj);

		template<class S, class U>
		static void centerHorizontally(const sf::Vector2<U> &size, S &obj);
	};

} // namespace ml
#include "../../../src/Utilities/Align.tpp"
#endif // PORTFOLIO_POSITIONABLE_H