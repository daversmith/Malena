//
// SegmentTheme.h
//

#ifndef SEGMENTTHEME_H
#define SEGMENTTHEME_H

#pragma once

#include <Malena/Traits/Theme/ToggleTheme.h>

namespace ml
{
    // ── SegmentTheme ──────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens for the @c SegmentToggle (Hotels/Apartments style).
     * @ingroup Settings
     *
     * Extends @c ToggleTheme with color tokens for the segmented selector —
     * the track background, the sliding thumb pill, and the active/inactive
     * option label colors.
     *
     * @see ToggleTheme, SegmentSettings, SegmentStyle, SegmentToggle
     */
    struct SegmentTheme : ToggleTheme
    {
        sf::Color trackColor        = sf::Color(60,  60,  60);
        sf::Color thumbColor        = sf::Color(100, 60,  200);
        sf::Color activeTextColor   = sf::Color::White;
        sf::Color inactiveTextColor = sf::Color(120, 120, 120);

        // ── applyFrom ─────────────────────────────────────────────────────────

        SegmentTheme& applyFrom(const Theme& t) override
        {
            ToggleTheme::applyFrom(t);
            trackColor        = t.surface;
            thumbColor        = t.primary;
            activeTextColor   = t.onPrimary;
            inactiveTextColor = t.muted;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        SegmentTheme& operator=(const ToggleTheme& t)
        {
            static_cast<ToggleTheme&>(*this) = t;
            return *this;
        }

        SegmentTheme& operator=(const ControlTheme& c)
        {
            static_cast<ToggleTheme&>(*this) = c;
            return *this;
        }

        SegmentTheme& operator=(const GraphicTheme& g)
        {
            static_cast<ToggleTheme&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getTrackColor()        const { return trackColor; }
        [[nodiscard]] sf::Color getThumbColor()        const { return thumbColor; }
        [[nodiscard]] sf::Color getActiveTextColor()   const { return activeTextColor; }
        [[nodiscard]] sf::Color getInactiveTextColor() const { return inactiveTextColor; }

        void setTrackColor(const sf::Color& c)        { trackColor        = c; }
        void setThumbColor(const sf::Color& c)        { thumbColor        = c; }
        void setActiveTextColor(const sf::Color& c)   { activeTextColor   = c; }
        void setInactiveTextColor(const sf::Color& c) { inactiveTextColor = c; }
    };

} // namespace ml

#endif // SEGMENTTHEME_H
