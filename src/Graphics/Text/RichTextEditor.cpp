// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Text/RichTextEditor.h>
#include <Malena/Utilities/Align.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>

#include <algorithm>
#include <string>

namespace
{
    // Encode a codepoint as UTF-8 (for icon-font glyph labels).
    std::string iconU8(char32_t cp)
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
}

namespace ml
{
    RichTextEditor::RichTextEditor()
    {
        // Toolbar colors are left to the active theme (the app decides light vs
        // dark). ToolbarTheme::itemTextColor makes a light theme possible.

        buildBar(false);   // start expanded; layout() collapses it when narrow

        // Alignment + list options for the collapsed (hamburger) state. Populated
        // once; opened from the menu button when the toolbar is too narrow to show
        // the inline icons.
        _alignMenu.addItem("Align left",    [this] { setParagraphAlign(0); });
        _alignMenu.addItem("Align center",  [this] { setParagraphAlign(1); });
        _alignMenu.addItem("Align right",   [this] { setParagraphAlign(2); });
        _alignMenu.addItem("Bullet list",   [this] { toggleList(1); });
        _alignMenu.addItem("Numbered list", [this] { toggleList(2); });

        // Color swatch — a small box filled with the last-chosen color; toggles
        // the ColorChooser pop-up; choosing a color applies it to the selection.
        _colorBtn.setSize({ 26.f, 26.f });
        _colorBtn.setColor(sf::Color(30, 30, 30));
        // The swatch always shows the color at the cursor — i.e. what you'll
        // type — so it tracks the caret as it moves between differently-colored
        // runs (uses the char before the caret, which is what typing inherits).
        _colorBtn.setColorProvider([this] {
            const std::size_t c = _area.getCursor();
            return _area.getColorAt(c > 0 ? c - 1 : c);
        });
        _colorBtn.onClick([this] {
            const bool open = !_color.isOpen();
            if (open) {                       // capture the selection BEFORE the
                _colorSelStart = _area.getSelectionStart();   // pop-up steals focus
                _colorSelEnd   = _area.getSelectionEnd();
            }
            _color.setOpen(open);
            // The pop-up overlays the text area; disable the area's mouse while
            // open so dragging in the picker doesn't start a text selection.
            _area.setEnabled(!open);
        });
        // Live preview while dragging the gradient — apply, keep the pop-up open.
        _color.onColorChanged([this](const sf::Color& c) {
            _area.setSelection(_colorSelStart, _colorSelEnd);
            _area.setSelectionColor(c);
            _colorBtn.setColor(c);            // swatch reflects the chosen color
        });
        // Committed pick (swatch click) — apply and close.
        _color.onColorSelected([this](const sf::Color& c) {
            _area.setSelection(_colorSelStart, _colorSelEnd);
            _area.setSelectionColor(c);
            _colorBtn.setColor(c);
            _color.setOpen(false);
            _area.setEnabled(true);
        });

        // Font size — a compact type-in box (smaller than a dropdown). It mirrors
        // the size at the caret (see the onUpdate sync below) and applies the typed
        // value, clamped to [kMinFontSize, kMaxFontSize], on Enter.
        _sizeInput.setSize({ 46.f, 28.f });
        _sizeInput.setMaxLength(3);                       // up to 3 digits
        _sizeInput.setClearSelectionOnBlur(false);
        _sizeInput.setValue(std::to_string(_area.getCharacterSize()));
        _sizeInput.onSubmit([this](const std::string& v) {
            int n = 0;
            try { n = std::stoi(v); } catch (...) { n = 0; }
            if (n <= 0) {   // invalid entry — restore the caret's current size
                _sizeInput.setValue(std::to_string(
                    _area.getActiveStyle().charSize.value_or(_area.getCharacterSize())));
                return;
            }
            const unsigned int clamped = std::clamp<unsigned int>(
                static_cast<unsigned int>(n), kMinFontSize, kMaxFontSize);
            _area.setSelectionCharSize(clamped);          // applies to selection or next typing
            _sizeInput.setValue(std::to_string(clamped)); // reflect the clamp
        });

        // Headings dropdown — block-level: applies to the whole paragraph the
        // cursor is in. H1-H4 = larger + bold; Code = monospace; Paragraph = reset.
        for (const char* h : { "Paragraph", "H1", "H2", "H3", "H4", "Code" })
            _headingSelect.addOption(h);
        _headingSelect.setPlaceholder("Style");
        _headingSelect.setSize({ 84.f, 28.f });   // compact (default Select is 200 wide)
        _headingSelect.setItemHeight(26.f);
        _headingSelect.setMaxVisibleItems(6.f);
        _headingSelect.onOpen([this] {
            _hdSelStart = _area.getSelectionStart();
            _hdSelEnd   = _area.getSelectionEnd();
            if (_hdSelStart == _hdSelEnd) {
                const std::size_t c = _area.getCursor();
                _hdSelStart = _hdSelEnd = c;
            }
            _area.setEnabled(false);
        });
        _headingSelect.onClose([this] { _area.setEnabled(true); });
        _headingSelect.onSelectionChanged([this](const std::string& label, std::size_t) {
            if (_syncing) return;   // programmatic mirror of the caret — not a user pick
            // Expand the captured anchor to its paragraph (between newlines).
            const std::string text = _area.getValue();
            std::size_t a = std::min(_hdSelStart, text.size());
            std::size_t b = std::min(_hdSelEnd,   text.size());
            while (a > 0 && text[a - 1] != '\n') --a;
            while (b < text.size() && text[b] != '\n') ++b;
            _area.setSelection(a, b);

            const unsigned int base = _area.getCharacterSize();
            const sf::Font&    deflt = FontManager<>::getDefault();

            unsigned int size = base; bool bold = false; const sf::Font* font = &deflt;
            if      (label == "H1")   { size = 32; bold = true; }
            else if (label == "H2")   { size = 28; bold = true; }
            else if (label == "H3")   { size = 24; bold = true; }
            else if (label == "H4")   { size = 20; bold = true; }
            else if (label == "Code") { font = &CodeEditor::monospaceFont(); }
            // "Paragraph" → defaults

            _area.setSelectionFont(*font);
            _area.setSelectionCharSize(size);
            _area.setSelectionBold(bold);
            if (label != "Code")
                _area.setSelectionColor(_area.getTextColor());  // clear any old code colors

            // Color-code now (and onChange keeps code paragraphs live).
            rehighlightCode();
        });

        // Keep the selection alive when the user clicks a toolbar control
        // (bold/italic/color/size) — otherwise blur wipes it before we can style.
        _area.setClearSelectionOnBlur(false);

        // Register the code font so inline "Code" runs survive getRichText/
        // setRichText round-trips (serialized as "mono":true) and re-highlight.
        _area.setMonospaceFont(&CodeEditor::monospaceFont());

        _area.onChange([this](const std::string& t) {
            rehighlightCode();                 // keep code paragraphs live-colored
            if (_onChange) _onChange(t);
        });

        // Live-sync ALL toolbar controls to the caret while the editing surface is
        // focused (so they don't fight the user interacting with the controls).
        onUpdate([this] {
            if (_autoHideToolbar) updateActiveState();
            if (_area.isFocused()) syncControlsToCursor();
        });

        addUntracked(_bar);
        addUntracked(_colorBtn);
        addUntracked(_area);
        addUntracked(_color);
        addUntracked(_headingSelect);
        addUntracked(_sizeInput);
        addUntracked(_alignMenu);
        _color.setOpen(false);   // start closed
        layout();
    }

    void RichTextEditor::layout()
    {
        const sf::Vector2f p = getPosition();
        const sf::Vector2f s = getSize();

        // Responsive: collapse alignment + list into a hamburger when the editor
        // is too narrow to show the full inline set. Rebuild the bar only when the
        // mode actually flips (layout runs on resize, not every frame).
        const bool wantCompact = (s.x < kCompactBelow);
        if (wantCompact != _compact) { _compact = wantCompact; buildBar(_compact); }

        _bar.setPosition(p);
        _bar.setBarLength(s.x);   // match the editor width, not the whole window

        // Auto-hide mode: when not active (caret not in the editor), the toolbar
        // and its controls are parked off-screen + disabled so the editor reads as
        // a plain text area, and the text fills the full height.
        const bool chrome = (!_autoHideToolbar || _active);
        const sf::Vector2f OFF{ -100000.f, -100000.f };
        auto park = [&](auto& w) { w.setEnabled(false); w.setPosition(OFF); };

        // Reserve enough height for the toolbar AND the dropdown trigger so the
        // trigger never overhangs into (and steals clicks from) the text area.
        const float barH    = std::max(34.f, _bar.getGlobalBounds().size.y);
        const float reserve = chrome ? std::max(barH, 46.f) : 0.f;

        _area.setPosition({ p.x, p.y + reserve });
        _area.setSize({ s.x, std::max(0.f, s.y - reserve) });

        if (!chrome)
        {
            park(_bar); park(_colorBtn); park(_headingSelect); park(_sizeInput);
            _color.setOpen(false);
            return;
        }

        _bar.setEnabled(true); _colorBtn.setEnabled(true);
        _headingSelect.setEnabled(true); _sizeInput.setEnabled(true);
        _bar.setPosition(p);

        // Toolbar row, left→right, chained with Align so widths never overlap:
        // [B/I/U | align | list  icons in _bar] | color swatch | Style | Size.
        // The bar spans the full strip, so we can't Align off it directly — place
        // the swatch just past the bar's measured content, then chain the rest.
        _colorBtn.setPosition({ p.x + _bar.getContentExtent() + 6.f, p.y + 6.f });
        Align::setRightOf(_colorBtn,      _headingSelect, 8.f);
        Align::setRightOf(_headingSelect, _sizeInput,     6.f);

        // Color pop-up overlays the top-left of the text area, just under the bar.
        _color.setPosition({ p.x + 6.f, p.y + reserve + 2.f });
        _color.setSize({ 300.f, 330.f });
    }

    void RichTextEditor::setPosition(const sf::Vector2f& pos)
    {
        Panel::setPosition(pos);
        layout();
    }

    void RichTextEditor::setSize(const sf::Vector2f& size)
    {
        Panel::setSize(size);
        layout();
    }

    void RichTextEditor::closePopups()
    {
        _color.setOpen(false);
    }

    void RichTextEditor::setVisible(bool visible)
    {
        Panel::setVisible(visible);
        // Panel::setVisible propagates visibility to ALL children — including the
        // ColorChooser's internals — which bypasses its own _open gating. Re-assert
        // a clean state on every show/hide so a swapped-out editor can't come back
        // with a stray/mispainted pop-up, and collapse when hidden.
        if (!visible) _active = false;
        closePopups();
        layout();
    }

    void RichTextEditor::setAutoHideToolbar(bool enabled)
    {
        _autoHideToolbar = enabled;
        _active = false;     // start collapsed (reads as a plain text area)
        layout();
    }

    void RichTextEditor::updateActiveState()
    {
        const auto& win = WindowManager::getWindow();
        const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        // On a fresh press, activate iff the click landed inside the editor (its
        // area or toolbar); clicking outside collapses it. Keep active while the
        // color pop-up is open (it can extend past the editor's bounds).
        if (down && !_prevMouseDown)
        {
            const bool inside = getGlobalBounds().contains(wp) || _color.isOpen();
            if (inside != _active) { _active = inside; layout(); }
        }
        _prevMouseDown = down;
    }

    void RichTextEditor::highlightCodeRange(std::size_t a, std::size_t b, const std::string& text)
    {
        _area.setSelection(a, b);
        _area.setSelectionColor(_area.getTextColor());   // reset paragraph to base color
        BasicSyntaxHighlighter hl(CodeLanguage::Cpp);
        const std::string code = text.substr(a, b - a);
        for (const SyntaxToken& tok : hl.tokenize(code))
        {
            _area.setSelection(a + tok.start, a + tok.end);
            _area.setSelectionColor(tok.color);
        }
    }

    void RichTextEditor::rehighlightCode()
    {
        if (_inCodeHl) return;
        const sf::Font*   mono   = &CodeEditor::monospaceFont();
        const std::string text   = _area.getValue();
        const std::size_t cursor = _area.getCursor();

        bool any = false;
        _inCodeHl = true;
        std::size_t i = 0;
        while (i <= text.size())
        {
            std::size_t a = i, b = i;
            while (b < text.size() && text[b] != '\n') ++b;
            if (b > a && _area.getFontAt(a) == mono)   // a code paragraph
            {
                highlightCodeRange(a, b, text);
                any = true;
            }
            i = b + 1;
        }
        if (any) _area.setSelection(cursor, cursor);   // restore caret where the user is typing
        _inCodeHl = false;
    }

    void RichTextEditor::applyToParagraph(const std::function<void()>& fn)
    {
        // Expand the current selection/caret to whole paragraph(s), apply, then
        // put the caret back where the user was so typing continues normally.
        const std::string text = _area.getValue();
        std::size_t a = _area.getSelectionStart();
        std::size_t b = _area.getSelectionEnd();
        if (a == b) a = b = _area.getCursor();
        while (a > 0 && text[a - 1] != '\n') --a;
        while (b < text.size() && text[b] != '\n') ++b;

        const std::size_t caret = _area.getCursor();
        _area.setSelection(a, b);
        fn();
        // Restoring the caret clears the pending style — only do it when we
        // actually widened to a real selection. For an empty paragraph the caret
        // is already in place and the style lives in pending (previewed by the
        // renderer, applied on the next keystroke), so leave it untouched.
        if (a != b) _area.setSelection(caret, caret);
    }

    void RichTextEditor::buildBar(bool compact)
    {
        _bar.clear();

        // Bold / italic / underline — always shown.
        _iBold = _bar.addButton(iconU8(Icon::FormatBold), [this] {
            _b = !_b; _area.setSelectionBold(_b);   _bar.setItemSelected(_iBold, _b);
        });
        _iItalic = _bar.addButton(iconU8(Icon::FormatItalic), [this] {
            _i = !_i; _area.setSelectionItalic(_i); _bar.setItemSelected(_iItalic, _i);
        });
        _iUnderline = _bar.addButton(iconU8(Icon::FormatUnderlined), [this] {
            _u = !_u; _area.setSelectionUnderline(_u); _bar.setItemSelected(_iUnderline, _u);
        });
        _bar.setItemSelected(_iBold,      _b);   // restore toggle highlights
        _bar.setItemSelected(_iItalic,    _i);
        _bar.setItemSelected(_iUnderline, _u);

        _bar.addSeparator();

        if (compact)
        {
            // Collapsed: align + list move into a hamburger pop-up menu.
            _iMenu = _bar.addButton(iconU8(Icon::Menu), [this] { openAlignListMenu(); });
        }
        else
        {
            _iAlignL = _bar.addButton(iconU8(Icon::FormatAlignLeft),   [this] { setParagraphAlign(0); });
            _iAlignC = _bar.addButton(iconU8(Icon::FormatAlignCenter), [this] { setParagraphAlign(1); });
            _iAlignR = _bar.addButton(iconU8(Icon::FormatAlignRight),  [this] { setParagraphAlign(2); });
            _bar.setItemSelected(_iAlignL, _curAlign == 0);
            _bar.setItemSelected(_iAlignC, _curAlign == 1);
            _bar.setItemSelected(_iAlignR, _curAlign == 2);

            _bar.addSeparator();
            _iListBullet = _bar.addButton(iconU8(Icon::FormatListBulleted), [this] { toggleList(1); });
            _iListNum    = _bar.addButton(iconU8(Icon::FormatListNumbered), [this] { toggleList(2); });
            _bar.setItemSelected(_iListBullet, _curList == 1);
            _bar.setItemSelected(_iListNum,    _curList == 2);
        }
    }

    void RichTextEditor::openAlignListMenu()
    {
        // Drop the menu just under the hamburger (the bar's last item).
        const sf::Vector2f p = getPosition();
        _alignMenu.showAt({ p.x + _bar.getContentExtent() - 36.f, p.y + 34.f });
    }

    void RichTextEditor::syncControlsToCursor()
    {
        // Every control mirrors the style that applies at the caret right now.
        const TextAttribute a = _area.getActiveStyle();
        const unsigned int  sz   = a.charSize.value_or(_area.getCharacterSize());
        const bool          bold = a.bold.value_or(false);

        // Size box.
        const std::string szStr = std::to_string(sz);
        if (_sizeInput.getValue() != szStr) _sizeInput.setValue(szStr);

        // Bold / italic / underline toggles (B/I/U are always in the bar).
        _b = bold;
        _i = a.italic.value_or(false);
        _u = a.underline.value_or(false);
        _bar.setItemSelected(_iBold,      _b);
        _bar.setItemSelected(_iItalic,    _i);
        _bar.setItemSelected(_iUnderline, _u);

        // Alignment + list (inline highlights exist only when expanded).
        _curAlign = a.align.value_or(0);
        _curList  = a.listType.value_or(0);
        if (!_compact)
        {
            _bar.setItemSelected(_iAlignL, _curAlign == 0);
            _bar.setItemSelected(_iAlignC, _curAlign == 1);
            _bar.setItemSelected(_iAlignR, _curAlign == 2);
            _bar.setItemSelected(_iListBullet, _curList == 1);
            _bar.setItemSelected(_iListNum,    _curList == 2);
        }

        // Style dropdown — reverse-map the run to a heading preset.
        std::string style = "Paragraph";
        if      (a.font == &CodeEditor::monospaceFont()) style = "Code";
        else if (bold && sz == 32) style = "H1";
        else if (bold && sz == 28) style = "H2";
        else if (bold && sz == 24) style = "H3";
        else if (bold && sz == 20) style = "H4";
        if (style != _curStyle)
        {
            _curStyle = style;
            _syncing = true;                  // suppress the heading callback
            _headingSelect.selectValue(style);
            _syncing = false;
        }
    }

    void RichTextEditor::setParagraphAlign(int align)
    {
        applyToParagraph([this, align] { _area.setSelectionAlign(align); });
        _curAlign = align;
        if (!_compact) {   // inline align buttons only exist when expanded
            _bar.setItemSelected(_iAlignL, align == 0);
            _bar.setItemSelected(_iAlignC, align == 1);
            _bar.setItemSelected(_iAlignR, align == 2);
        }
    }

    void RichTextEditor::toggleList(int listType)
    {
        // Click an active list icon to turn the list off; otherwise switch to it.
        const int next = (_curList == listType) ? 0 : listType;
        applyToParagraph([this, next] { _area.setSelectionListType(next); });
        _curList = next;
        if (!_compact) {   // inline list buttons only exist when expanded
            _bar.setItemSelected(_iListBullet, _curList == 1);
            _bar.setItemSelected(_iListNum,    _curList == 2);
        }
    }

    void RichTextEditor::setValue(const std::string& text)    { _area.setValue(text); }
    std::string RichTextEditor::getValue() const              { return _area.getValue(); }
    void RichTextEditor::setRichText(const std::string& json) { _area.setRichText(json); }
    std::string RichTextEditor::getRichText() const           { return _area.getRichText(); }
    void RichTextEditor::onChange(std::function<void(const std::string&)> cb)
    { _onChange = std::move(cb); }

} // namespace ml
