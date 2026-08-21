// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Controls/ColorChooser.h>
#include <Malena/Utilities/Align.h>

#include <algorithm>
#include <cstdio>
#include <string>

namespace ml
{
    namespace
    {
        // Encode a codepoint as UTF-8 (for icon-font glyph labels).
        std::string u8(char32_t cp)
        {
            std::string s;
            if (cp < 0x80) s += static_cast<char>(cp);
            else if (cp < 0x800) {
                s += static_cast<char>(0xC0 | (cp >> 6));
                s += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                s += static_cast<char>(0xE0 | (cp >> 12));
                s += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                s += static_cast<char>(0x80 | (cp & 0x3F));
            }
            return s;
        }

        sf::Color parseHex(std::string h, bool& ok)
        {
            if (!h.empty() && h.front() == '#') h.erase(h.begin());
            if (h.size() != 6) { ok = false; return {}; }
            try {
                const unsigned long v = std::stoul(h, nullptr, 16);
                ok = true;
                return sf::Color(static_cast<std::uint8_t>((v >> 16) & 0xFF),
                                 static_cast<std::uint8_t>((v >>  8) & 0xFF),
                                 static_cast<std::uint8_t>( v        & 0xFF));
            } catch (...) { ok = false; return {}; }
        }

        std::uint8_t clamp255(const std::string& s)
        {
            try { return static_cast<std::uint8_t>(std::clamp(std::stoi(s), 0, 255)); }
            catch (...) { return 0; }
        }
    }

    std::string ColorChooser::toHex(const sf::Color& c)
    {
        char buf[8];
        std::snprintf(buf, sizeof buf, "#%02X%02X%02X", c.r, c.g, c.b);
        return buf;
    }

    ColorChooser::ColorChooser()
    {
        _iColor  = _tabs.addButton("Color",    [this] { showTab(0); });
        _iSwatch = _tabs.addButton("Swatches", [this] { showTab(1); });

        _gradient.onColorChanged([this](const sf::Color& c) { setCurrent(c, true); });
        _gradient.onCommit([this] { commit(_current); });   // Enter on the gradient commits

        _swatches.setColumns(8);
        _swatches.setSwatchSize(22.f);
        _swatches.setMaxColors(24);
        _swatches.onColorSelected([this](const sf::Color& c) { setCurrent(c, true); commit(c); });

        _recent.setColumns(8);
        _recent.setSwatchSize(22.f);
        _recent.setMaxColors(16);
        _recent.setPalette({});   // recents start empty
        _recent.onColorSelected([this](const sf::Color& c) { setCurrent(c, true); commit(c); });

        // Right-click a swatch → context menu. Disable the pickers while the menu
        // is open so the click that selects a menu item doesn't also hit a swatch.
        _swatches.onSwatchRightClick([this](std::size_t i, const sf::Vector2f& pos) {
            _swatches.setEnabled(false); _recent.setEnabled(false);
            _menu.clearItems();
            _menu.addItem("Delete", [this, i] { _swatches.removeColor(i); });
            _menu.showAt(pos);
        });
        _recent.onSwatchRightClick([this](std::size_t i, const sf::Vector2f& pos) {
            _swatches.setEnabled(false); _recent.setEnabled(false);
            _menu.clearItems();
            _menu.addItem("Remove", [this, i] { _recent.removeColor(i); });
            _menu.showAt(pos);
        });
        _menu.onClose([this] { _swatches.setEnabled(true); _recent.setEnabled(true); });

        // Mode toggle (RGB | HEX) — a real SegmentToggle.
        {
            SegmentSettings ss;
            ss.setSegmentLabels("RGB", "HEX");
            ss.setSize({ 96.f, 24.f });
            _modeToggle.applySettings(ss);
        }
        _modeToggle.onToggled([this](bool on) { setHexMode(on); });

        // Actions as icon buttons (Material Icons font): 💾 Save, ✓ Done.
        _actionBar.setItemSize({ 34.f, 24.f });
        _iSave = _actionBar.addButton(u8(Icon::Save), [this] {
            _swatches.addColor(_current);
            showTab(1);
        });
        _iDone = _actionBar.addButton(u8(Icon::Check), [this] { commit(_current); });

        _hexInput.setPlaceholder("#RRGGBB");
        _hexInput.onSubmit([this](const std::string& s) {
            bool ok; const sf::Color c = parseHex(s, ok);
            if (ok) { setCurrent(c, true); commit(c); }
        });

        auto rgbSubmit = [this](const std::string&) {
            const sf::Color c(clamp255(_rInput.getValue()),
                              clamp255(_gInput.getValue()),
                              clamp255(_bInput.getValue()));
            setCurrent(c, true); commit(c);
        };
        _rInput.setPlaceholder("R"); _rInput.onSubmit(rgbSubmit);
        _gInput.setPlaceholder("G"); _gInput.onSubmit(rgbSubmit);
        _bInput.setPlaceholder("B"); _bInput.onSubmit(rgbSubmit);

        addUntracked(_tabs);
        addUntracked(_gradient);
        addUntracked(_swatches);
        addUntracked(_recent);
        addUntracked(_modeToggle);
        addUntracked(_hexInput);
        addUntracked(_rInput);
        addUntracked(_gInput);
        addUntracked(_bInput);
        addUntracked(_actionBar);
        addUntracked(_menu);     // last → draws on top of everything

        setSize({ 300.f, 330.f });
        _modeToggle.setOn(true);   // start in HEX mode (fires onToggled → setHexMode)
        showTab(0);
        syncInputs();
    }

    void ColorChooser::onColorSelected(std::function<void(const sf::Color&)> cb)
    { _onColorSelected = std::move(cb); }
    void ColorChooser::onColorChanged(std::function<void(const sf::Color&)> cb)
    { _onColorChanged = std::move(cb); }

    void ColorChooser::setMaxSwatches(int max) { _swatches.setMaxColors(max); }

    void ColorChooser::setColor(const sf::Color& c) { setCurrent(c, false); }

    void ColorChooser::setCurrent(const sf::Color& c, bool notify)
    {
        _current = c;
        _gradient.setColor(c);
        syncInputs();
        if (notify && _onColorChanged) _onColorChanged(c);
    }

    void ColorChooser::commit(const sf::Color& c)
    {
        recordRecent(c);
        if (_onColorSelected) _onColorSelected(c);
    }

    void ColorChooser::recordRecent(const sf::Color& c)
    {
        const auto& pal = _recent.palette();
        for (std::size_t i = 0; i < pal.size(); ++i)
            if (pal[i] == c) { _recent.removeColor(i); break; }   // dedupe
        _recent.addColor(c);   // newest at end; capped to 16
    }

    void ColorChooser::syncInputs()
    {
        _hexInput.setValue(toHex(_current));
        _rInput.setValue(std::to_string(_current.r));
        _gInput.setValue(std::to_string(_current.g));
        _bInput.setValue(std::to_string(_current.b));
    }

    void ColorChooser::setHexMode(bool hex)
    {
        _hexMode = hex;
        applyVisibility();   // toggle widget reflects its own state
    }

    void ColorChooser::setLayout(Layout layout)
    {
        _layout = layout;
        relayout();
        applyVisibility();
    }

    void ColorChooser::showTab(int index)
    {
        _active = index;
        _tabs.setItemSelected(_iColor,  index == 0);
        _tabs.setItemSelected(_iSwatch, index == 1);
        relayout();
        applyVisibility();
    }

    void ColorChooser::setOpen(bool open)
    {
        _open = open;
        if (open) disableFlag(ml::Flag::HIDDEN);
        else      enableFlag(ml::Flag::HIDDEN);
        applyVisibility();
    }

    void ColorChooser::applyVisibility()
    {
        auto show = [](Core& c, bool v) {
            if (v) c.disableFlag(ml::Flag::HIDDEN); else c.enableFlag(ml::Flag::HIDDEN);
        };

        Core* all[] = { &_tabs, &_gradient, &_swatches, &_recent, &_modeToggle,
                        &_hexInput, &_rInput, &_gInput, &_bInput, &_actionBar };
        if (!_open) { for (Core* c : all) show(*c, false); _menu.hide(); return; }

        bool gradVis = false, swVis = false;
        switch (_layout)
        {
        case Layout::Tabbed:       gradVis = (_active == 0); swVis = (_active == 1); break;
        case Layout::Stacked:      gradVis = true;           swVis = true;           break;
        case Layout::GradientOnly: gradVis = true;                                   break;
        case Layout::SwatchesOnly: swVis   = true;                                   break;
        }

        show(_tabs, _layout == Layout::Tabbed);
        show(_gradient, gradVis);
        show(_swatches, swVis);
        show(_recent,   swVis);

        // RGB/Hex entry + Save/Done belong to the Color (gradient) view.
        show(_modeToggle, gradVis);
        show(_actionBar,  gradVis);
        show(_hexInput,  gradVis && _hexMode);
        show(_rInput,    gradVis && !_hexMode);
        show(_gInput,    gradVis && !_hexMode);
        show(_bInput,    gradVis && !_hexMode);
    }

    void ColorChooser::relayout()
    {
        const sf::Vector2f p = getPosition();
        const sf::Vector2f s = getSize();
        const float pad = 6.f;

        const bool  tabbed = (_layout == Layout::Tabbed);
        const float tabH   = tabbed ? std::max(30.f, _tabs.getGlobalBounds().size.y) + 4.f : 0.f;
        if (tabbed) { _tabs.setPosition(p); _tabs.setBarLength(s.x); }

        const float rowH   = 26.f;
        const float actionY = p.y + s.y - rowH - pad;          // [Save][Done]
        const float entryY  = actionY - rowH - 4.f;            // toggle + fields

        // ── Color-tab bottom rows (Align places fields past the toggle by its
        //    real bounds, so nothing overlaps; both hex and rgb are positioned
        //    so toggling RGB/HEX needs no relayout). ───────────────────────────
        _modeToggle.setPosition({ p.x + pad, entryY });

        // Hex field fills the remaining width to the right of the toggle.
        Align::setRightOf(_modeToggle, _hexInput, 12.f);
        _hexInput.setSize({ std::max(40.f, (p.x + s.x - pad) - _hexInput.getPosition().x),
                            rowH - 2.f });

        // R/G/B fields: fixed width, chained after the toggle.
        const float each = 44.f;
        _rInput.setSize({ each, rowH - 2.f });
        _gInput.setSize({ each, rowH - 2.f });
        _bInput.setSize({ each, rowH - 2.f });
        Align::setRightOf(_modeToggle, _rInput, 12.f);
        Align::setRightOf(_rInput,     _gInput, 4.f);
        Align::setRightOf(_gInput,     _bInput, 4.f);

        _actionBar.setPosition({ p.x + pad, actionY });
        _actionBar.setBarLength(2.f * 34.f + 6.f);

        // ── Gradient (Color tab) fills between tabs and the entry rows ────────
        const sf::Vector2f cPos { p.x, p.y + tabH };
        _gradient.setPosition(cPos);
        _gradient.setSize({ s.x, std::max(0.f, (entryY - 6.f) - cPos.y) });

        // ── Swatches tab: saved grid, then recent grid below ─────────────────
        _swatches.setPosition({ p.x + pad, p.y + tabH + 6.f });
        _recent.setPosition({ p.x + pad,
                              p.y + tabH + 6.f + _swatches.getSize().y + 18.f });
    }

    void ColorChooser::setPosition(const sf::Vector2f& pos) { Panel::setPosition(pos); relayout(); }
    void ColorChooser::setSize(const sf::Vector2f& size)    { Panel::setSize(size);    relayout(); }

} // namespace ml
