//
// GraphicTheme.h
//

#ifndef GRAPHICTHEME_H
#define GRAPHICTHEME_H

#pragma once

#include <Malena/Manifests/Theme.h>
#include <SFML/Graphics/Color.hpp>

namespace ml
{
    // ── GraphicTheme ──────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens for all visual components.
     * @ingroup Settings
     *
     * Maps universal @c Theme tokens to the lowest-level visual variables
     * shared by every drawable component. All other theme structs inherit
     * from this.
     *
     * Getters and setters are defined here so any component that inherits
     * @c GraphicTheme exposes them directly on its own API.
     *
     * ### Inheritance chain
     * @code
     * GraphicTheme
     *     └── ControlTheme
     *             └── ToggleTheme
     *                     └── PillTheme
     *                     └── SegmentTheme
     *                     └── ButtonTheme
     * @endcode
     *
     * @see ControlTheme, Theme, ThemeManager
     */
    struct GraphicTheme
    {
        sf::Color fillColor        = sf::Color(40,  40,  40);
        sf::Color outlineColor     = sf::Color(100, 100, 100);
        float     outlineThickness = 1.5f;
        float     radius           = 8.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        /**
         * @brief Populate all fields from the global @c Theme token set.
         *
         * Subclasses call @c GraphicTheme::applyFrom(t) first, then
         * apply their own token mappings.
         */
        virtual GraphicTheme& applyFrom(const Theme& t)
        {
            fillColor        = t.surface;
            outlineColor     = t.border;
            outlineThickness = t.borderThickness;
            radius           = t.radius;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getFillColor()        const { return fillColor; }
        [[nodiscard]] sf::Color getOutlineColor()     const { return outlineColor; }
        [[nodiscard]] float     getOutlineThickness() const { return outlineThickness; }
        [[nodiscard]] float     getRadius()           const { return radius; }

        void setFillColor(const sf::Color& c)    { fillColor        = c; }
        void setOutlineColor(const sf::Color& c) { outlineColor     = c; }
        void setOutlineThickness(float t)        { outlineThickness = t; }
        void setRadius(float r)                  { radius           = r; }

        // ── Assignment from parent ────────────────────────────────────────────
        // (base of the chain — no parent to assign from)

        virtual ~GraphicTheme() = default;
    };

} // namespace ml

#endif // GRAPHICTHEME_H
