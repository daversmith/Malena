//
// ThemeTag.h
//

#ifndef MALENA_THEMETAG_H
#define MALENA_THEMETAG_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <memory>
#include <unordered_map>
#include <Malena/Utilities/EnumClassHash.h>

namespace ml
{
    /**
     * @brief Lightweight polymorphic base for all Theme structs.
     *
     * Has no external dependencies — safe to include from Manifest.h
     * without pulling in FontManager or SFML. Manifest stores themes as
     * @c shared_ptr<ThemeTag>; ThemeManager casts back to @c Theme& on apply.
     *
     * @see Theme, Manifest, ThemeManager
     */
    struct ThemeTag
    {
        virtual ~ThemeTag() = default;
    };

} // namespace ml

#endif // MALENA_THEMETAG_H
