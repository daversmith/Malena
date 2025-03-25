//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_HELPER_H
#define PORTFOLIO_HELPER_H

#pragma once

#include <SFML/Graphics.hpp>

namespace ml
{
	class Helper
	{
	public:
		static void output(sf::Vector2f vector);
		static void output(sf::FloatRect floatRect);
	};

} // namespace ml
#endif // PORTFOLIO_HELPER_H
