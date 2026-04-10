//
// ToggleSettings.h
//

#ifndef MALENA_TOGGLESETTINGS_H
#define MALENA_TOGGLESETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>
#include <string>
#include <vector>
#include <utility>

namespace ml
{
    /**
     * @brief Settings shared by all toggle variants — behaviour only.
     * @ingroup Traits
     *
     * Colors live in @c ToggleTheme. @c ToggleSettings holds animation
     * speed and the item list used to pre-populate a @c ToggleGroup.
     *
     * @see ControlSettings, ToggleTheme, PillSettings
     */
    struct ToggleSettings : ControlSettings
    {
        float animSpeed = 10.f;
        std::vector<std::pair<std::string, std::string>> items;

        // ── Assignment from parent levels ─────────────────────────────────────

        ToggleSettings& operator=(const ControlSettings& c)
        {
            static_cast<ControlSettings&>(*this) = c;
            return *this;
        }

        ToggleSettings& operator=(const GraphicSettings& g)
        {
            static_cast<ControlSettings&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getAnimSpeed() const { return animSpeed; }
        void setAnimSpeed(float s)               { animSpeed = s; }

        [[nodiscard]] const std::vector<std::pair<std::string, std::string>>&
        getItems() const { return items; }

        void setItems(std::vector<std::pair<std::string, std::string>> i)
        { items = std::move(i); }

        void addItem(const std::string& label, const std::string& key)
        { items.emplace_back(label, key); }

        void clearItems() { items.clear(); }
    };

} // namespace ml

#endif // MALENA_TOGGLESETTINGS_H
