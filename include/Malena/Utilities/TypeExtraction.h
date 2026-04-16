// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/10/26.
//

#ifndef MALENA_TYPEEXTRACTION_H
#define MALENA_TYPEEXTRACTION_H

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

	/**
	 * @brief Extracts the @c manifest_type typedef from a trait, or @c void if absent.
	 *
	 * Used by @c GatherFlags and @c GatherStates to pull manifest enums
	 * out of trait types.
	 */
	template<typename T, typename = void>
	struct extract_TraitManifest { using type = void; };

	template<typename T>
	struct extract_TraitManifest<T, std::void_t<typename T::manifest_type>>
	{ using type = typename T::manifest_type; };

	/// @endcond

} // namespace ml

#endif // MALENA_TYPEEXTRACTION_H
