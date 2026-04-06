//
// ListItemStyle.h
//

#ifndef LISTITEMSTYLE_H
#define LISTITEMSTYLE_H

#pragma once

#include <Malena/Traits/Settings/ListItemSettings.h>
#include <Malena/Traits/Theme/ListItemTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c ListItem.
     * @ingroup Traits
     * @see ListItemSettings, ListItemTheme, ListItem
     */
    struct ListItemStyle : ListItemSettings, ListItemTheme
    {
        ListItemStyle& operator=(const ListItemSettings& s)
        { static_cast<ListItemSettings&>(*this) = s; return *this; }

        ListItemStyle& operator=(const ListItemTheme& t)
        { static_cast<ListItemTheme&>(*this) = t; return *this; }

        ListItemStyle& operator=(const ControlTheme& c)
        { static_cast<ListItemTheme&>(*this) = c; return *this; }

        ListItemStyle& operator=(const ControlSettings& c)
        { static_cast<ListItemSettings&>(*this) = c; return *this; }

        ListItemStyle& applyFrom(const Theme& t)
        { ListItemTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // LISTITEMSTYLE_H
