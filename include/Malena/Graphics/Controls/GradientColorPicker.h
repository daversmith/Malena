// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_GRADIENTCOLORPICKER_H
#define MALENA_GRADIENTCOLORPICKER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

namespace ml
{
    /**
     * @brief A Photoshop-style HSV color picker: a saturation/value gradient
     *        square plus a vertical hue bar. Click or drag to choose a color.
     * @ingroup GraphicsControls
     *
     * Reusable and self-contained (polls the mouse in its update loop, so it
     * works in any container). Pairs naturally with @c ColorPicker (swatches)
     * to offer both "Color" and "Swatches" pickers.
     *
     * @code
     * ml::GradientColorPicker picker;
     * picker.setSize({240.f, 200.f});
     * picker.onColorChanged([&](const sf::Color& c){ text.setSelectionColor(c); });
     * @endcode
     */
    class MALENA_API GradientColorPicker : public Graphic<sf::RectangleShape>
    {
    public:
        GradientColorPicker();

        /** Set the current color (updates the H/S/V handles). */
        void setColor(const sf::Color& c);
        [[nodiscard]] sf::Color color() const;

        /** Fired continuously while the color changes (click or drag). */
        void onColorChanged(std::function<void(const sf::Color&)> cb);
        /** Fired when Enter is pressed while visible — a "commit/select". */
        void onCommit(std::function<void()> cb);

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;

        void setPosition(const sf::Vector2f& pos) override;
        [[nodiscard]] sf::Vector2f  getPosition()     const override;
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // HSV<->RGB helpers (h in [0,360), s/v in [0,1]).
        static sf::Color hsvToRgb(float h, float s, float v);
        static void      rgbToHsv(const sf::Color& c, float& h, float& s, float& v);

    private:
        [[nodiscard]] sf::FloatRect svRect()  const;
        [[nodiscard]] sf::FloatRect hueRect() const;
        void emitChange();

        sf::Vector2f _pos  { 0.f, 0.f };
        sf::Vector2f _size { 240.f, 200.f };
        float        _hueBarW = 22.f;
        float        _gap     = 10.f;

        float _h = 0.f, _s = 1.f, _v = 1.f;

        int  _drag          = 0;   // 0 none, 1 SV square, 2 hue bar
        bool _prevMouseDown = false;
        bool _prevEnter     = false;

        std::function<void(const sf::Color&)> _onColorChanged;
        std::function<void()>                 _onCommit;
    };

} // namespace ml

#endif // MALENA_GRADIENTCOLORPICKER_H
