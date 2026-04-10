//
// ListStyle.h
//

#ifndef MALENA_LISTSTYLE_H
#define MALENA_LISTSTYLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ListSettings.h>
#include <Malena/Traits/Theme/ListTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c List.
     * @ingroup Traits
     * @see ListSettings, ListTheme, List
     */
    struct ListStyle : ListSettings, ListTheme
    {
        ListStyle& operator=(const ListSettings& s)
        { static_cast<ListSettings&>(*this) = s; return *this; }

        ListStyle& operator=(const ListTheme& t)
        { static_cast<ListTheme&>(*this) = t; return *this; }

        ListStyle& operator=(const ControlTheme& c)
        { static_cast<ListTheme&>(*this) = c; return *this; }

        ListStyle& operator=(const ControlSettings& c)
        { static_cast<ListSettings&>(*this) = c; return *this; }

        ListStyle& applyFrom(const Theme& t)
        { ListTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // MALENA_LISTSTYLE_H
