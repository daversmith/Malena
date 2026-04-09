// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/10/26.
//

#ifndef TYPEEXTRACTION_H
#define TYPEEXTRACTION_H

/**
 * @file TypeExtraction.h
 * @ingroup Utilities
 */

#include <Malena/Core/malena_export.h>

namespace ml
{
	/// @cond INTERNAL

	/**
	 * @brief Extracts the @c Flags enum from a type, or @c void if absent.
	 *
	 * Used by @c GatherFlags to detect flag enums on manifests and traits.
	 */
	template<typename T, typename = void>
	struct extract_Flags { using type = void; };

	template<typename T>
	struct extract_Flags<T, std::void_t<typename T::Flags>>
	{
		using type = typename T::Flags;
	};

	/**
	 * @brief Extracts the @c State enum from a type, or @c void if absent.
	 *
	 * Used by @c GatherStates and @c Customizable to detect state enums
	 * on manifests.
	 */
	template<typename T, typename = void>
	struct extract_State { using type = void; };

	template<typename T>
	struct extract_State<T, std::void_t<typename T::State>>
	{
		using type = typename T::State;
	};

	/// @endcond

} // namespace ml

#endif // TYPEEXTRACTION_H
