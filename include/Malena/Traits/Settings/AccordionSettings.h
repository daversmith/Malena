
#ifndef MALENA_ACCORDIONSETTINGS_H
#define MALENA_ACCORDIONSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct AccordionSettings : ControlSettings
    {
        float headerHeight  = 40.f;
        float chevronSize   = 8.f;
        float chevronRight  = 14.f;   ///< Right margin for the chevron
        bool  exclusive     = true;   ///< Only one section open at a time

        AccordionSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        AccordionSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        [[nodiscard]] float getHeaderHeight() const { return headerHeight; }
        [[nodiscard]] float getChevronSize()  const { return chevronSize;  }
        [[nodiscard]] bool  getExclusive()    const { return exclusive;    }

        void setHeaderHeight(float h) { headerHeight = h; }
        void setChevronSize(float s)  { chevronSize  = s; }
        void setExclusive(bool e)     { exclusive    = e; }
    };

} // namespace ml

#endif // MALENA_ACCORDIONSETTINGS_H
