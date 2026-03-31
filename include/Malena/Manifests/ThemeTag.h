//
// ThemeTag.h
//

#ifndef THEMETAG_H
#define THEMETAG_H

#pragma once

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

#endif // THEMETAG_H
