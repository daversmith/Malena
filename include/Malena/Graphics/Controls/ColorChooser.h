// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_COLORCHOOSER_H
#define MALENA_COLORCHOOSER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Graphics/Controls/Toolbar.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ColorPicker.h>
#include <Malena/Graphics/Controls/GradientColorPicker.h>
#include <Malena/Graphics/Controls/ContextMenu.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Resources/IconFont.h>
#include <cstddef>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief Combined color chooser: a "Color" (HSV gradient + RGB/Hex entry +
     *        Save/Done) tab and a "Swatches" tab (saved palette + recent colors).
     *
     * Saved swatches are user-managed: "Save color" on the Color tab adds the
     * current color (and jumps to Swatches); right-click a swatch to delete it;
     * the saved palette is capped (default 24, see @c setMaxSwatches). A second
     * "Recent" strip auto-records the last 16 committed colors.
     *
     * @see ColorPicker, GradientColorPicker
     */
    class MALENA_API ColorChooser : public Panel
    {
    public:
        enum class Layout { Tabbed, Stacked, GradientOnly, SwatchesOnly };

        ColorChooser();

        /** Fired on a deliberate pick — Done, a swatch/recent click, Hex/RGB submit. */
        void onColorSelected(std::function<void(const sf::Color&)> cb);
        /** Fired continuously as the color changes (gradient drag / live edits). */
        void onColorChanged(std::function<void(const sf::Color&)> cb);

        void setColor(const sf::Color& c);
        [[nodiscard]] sf::Color color() const { return _current; }

        /** Cap the saved-swatch palette (default 24). */
        void setMaxSwatches(int max);

        void setLayout(Layout layout);
        [[nodiscard]] Layout getLayout() const { return _layout; }
        void showTab(int index);

        void setOpen(bool open);
        [[nodiscard]] bool isOpen() const { return _open; }

        void setPosition(const sf::Vector2f& pos) override;
        void setSize(const sf::Vector2f& size);

    private:
        void relayout();
        void applyVisibility();
        void setHexMode(bool hex);
        void setCurrent(const sf::Color& c, bool notify);
        void commit(const sf::Color& c);        // deliberate pick → recent + onColorSelected
        void recordRecent(const sf::Color& c);
        void syncInputs();

        static std::string toHex(const sf::Color& c);

        Toolbar             _tabs;
        GradientColorPicker _gradient;
        ColorPicker         _swatches;   // saved palette (capped; right-click delete)
        ColorPicker         _recent;     // auto recent colors (max 16)

        SegmentToggle _modeToggle;           // RGB | HEX
        TextInput _hexInput;                 // "#RRGGBB"
        TextInput _rInput, _gInput, _bInput; // 0-255 each
        Toolbar   _actionBar{ iconFont() };  // [💾 save] [✓ done] (icon font)
        ContextMenu _menu;                   // right-click swatch menu (Delete)

        std::size_t _iSave = 0, _iDone = 0;

        Layout      _layout  = Layout::Tabbed;
        std::size_t _iColor  = 0, _iSwatch = 0;
        int         _active  = 0;
        bool        _open    = true;
        bool        _hexMode = true;

        sf::Color _current { 30, 30, 30 };

        std::function<void(const sf::Color&)> _onColorSelected;
        std::function<void(const sf::Color&)> _onColorChanged;
    };

} // namespace ml

#endif // MALENA_COLORCHOOSER_H
