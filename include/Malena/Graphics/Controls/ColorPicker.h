// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_COLORPICKER_H
#define MALENA_COLORPICKER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>
#include <vector>

namespace ml
{
    /**
     * @brief A reusable swatch grid for choosing a color.
     * @ingroup GraphicsControls
     *
     * Draws a grid of color swatches and fires @c onColorSelected when one is
     * clicked. Self-contained: it polls the mouse in its update loop (like the
     * text widgets), so it works anywhere — standalone, in a panel, or as a
     * pop-up under a toolbar button. Ships with a sensible default palette;
     * override with @c setPalette.
     *
     * @code
     * ml::ColorPicker picker;
     * picker.setColumns(6);
     * picker.onColorSelected([&](const sf::Color& c){ text.setSelectionColor(c); });
     * @endcode
     */
    class MALENA_API ColorPicker : public Graphic<sf::RectangleShape>
    {
    public:
        ColorPicker();

        /** Replace the palette (laid out left-to-right, wrapping by columns). */
        void setPalette(const std::vector<sf::Color>& colors);
        [[nodiscard]] const std::vector<sf::Color>& palette() const { return _palette; }

        /** Append a color to the palette (e.g. "save current color"). When the
         *  palette would exceed the max, the oldest swatch is dropped. */
        void addColor(const sf::Color& c);
        /** Remove the swatch at an index (no-op if out of range). */
        void removeColor(std::size_t index);

        /** Cap the number of swatches (0 = unlimited). Default 0. */
        void setMaxColors(int max);

        /** Fired when a swatch is removed. */
        void onColorRemoved(std::function<void(const sf::Color&)> cb);

        /** Fired on right-click of a swatch (index + world position) — e.g. to
         *  show a context menu. If unset, right-click does nothing. */
        void onSwatchRightClick(std::function<void(std::size_t, const sf::Vector2f&)> cb);

        void setColumns(int columns);
        void setSwatchSize(float px);
        void setSwatchGap(float px);

        /** Fired with the chosen color when a swatch is clicked. */
        void onColorSelected(std::function<void(const sf::Color&)> cb);

        // ── Positionable / size (auto-sized to the grid) ──────────────────────
        void setPosition(const sf::Vector2f& pos) override;
        [[nodiscard]] sf::Vector2f  getPosition()     const override;
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        sf::FloatRect cellRect(std::size_t index) const;
        void          recomputeSize();

        std::vector<sf::Color> _palette;
        int _maxColors = 0;   // 0 = unlimited
        std::function<void(const sf::Color&)> _onColorSelected;
        std::function<void(const sf::Color&)> _onColorRemoved;
        std::function<void(std::size_t, const sf::Vector2f&)> _onSwatchRightClick;

        sf::Vector2f _pos     { 0.f, 0.f };
        float        _swatch  = 22.f;
        float        _gap     = 4.f;
        float        _pad     = 6.f;
        int          _cols    = 8;

        bool         _prevMouseDown  = false;
        bool         _prevRightDown  = false;
        mutable int  _hoverIndex     = -1;
    };

} // namespace ml

#endif // MALENA_COLORPICKER_H
