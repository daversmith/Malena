// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_HELPER_H
#define PORTFOLIO_HELPER_H

#pragma once
/**
 * @file Helper.h
 * @ingroup Utilities
 */

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>

namespace ml
{
	/**
	 * @brief Static debug helpers for printing SFML value types.
	 * @ingroup Utilities
	 *
	 * @c Helper provides lightweight console-output utilities for the two
	 * SFML types most commonly inspected during development: @c sf::Vector2f
	 * and @c sf::FloatRect. Both methods write to @c stdout via
	 * @c std::cout and are intended for debugging only — they are not
	 * used by the framework at runtime.
	 *
	 * ### Usage
	 * @code
	 * ml::Helper::output(myRect.getPosition());
	 * ml::Helper::output(myRect.getGlobalBounds());
	 * @endcode
	 *
	 * @note For production logging, prefer a dedicated logging library.
	 */
	class MALENA_API Helper
	{
	public:
		/**
		 * @brief Print a @c sf::Vector2f to stdout.
		 *
		 * Outputs the vector in the form @c "(x, y)\n".
		 *
		 * @param vector The vector to print.
		 */
		static void output(sf::Vector2f vector);

		/**
		 * @brief Print a @c sf::FloatRect to stdout.
		 *
		 * Outputs the rect in the form @c "(left, top, width, height)\n".
		 *
		 * @param floatRect The rect to print.
		 */
		static void output(sf::FloatRect floatRect);
	};

} // namespace ml
#endif // PORTFOLIO_HELPER_H
