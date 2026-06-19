// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Utilities/Json.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    TextInput::TextInput(const sf::Font& font_)
        : _placeholder(font_),
          _maskDisplay(font_),
          _renderer(_buffer, font_, 16, sf::Color::White)
    {
        // Seed from active theme, then apply the constructor font
        TextInputTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        for (unsigned int sz : {11u, 12u, 14u, 16u, 18u, 20u, 24u})
            ml::FontManager<>::prewarm(font_, sz);

        _background.setSize(size);
        _background.setFillColor(bgIdle);
        _background.setOutlineColor(borderIdle);
        _background.setOutlineThickness(borderThickness);

        _placeholder.setCharacterSize(fontSize);
        FontManager<>::prewarm(font_, fontSize);
        _placeholder.setFillColor(placeholderColor);

        static_cast<void>(_canvas.resize({
            std::max(1u, static_cast<unsigned int>(size.x)),
            std::max(1u, static_cast<unsigned int>(size.y))
        }));

        rebuild();

        // ── Focus / blur ──────────────────────────────────────────────────────
        onFocus([this]{
            if (!checkFlag(ml::Flag::ENABLED)) return;
            setState(State::FOCUSED);
            syncColors();
            _cursorClock.restart();
            _cursorVisible = true;
        });

        onBlur([this]{
            _dragging = false;

            const sf::Vector2f mp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));

            if (!getGlobalBounds().contains(mp))
            {
                // Genuine blur — mouse left the field. Reset double-click state,
                // and clear selection unless an external toolbar needs it kept.
                _waitingDouble = false;
                if (_clearSelectionOnBlur) _buffer.clearSelection();
                reflow();
            }
            // If mouse IS still inside, this is the blur/focus cycle that
            // fires when clicking on an already-focused field. Preserve
            // _waitingDouble so double-click detection can still succeed.

            if (!isState(State::ERROR)) setState(State::IDLE);
            syncColors();
        });

        // ── Keyboard ──────────────────────────────────────────────────────────
        onKeypress([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (!checkFlag(ml::Flag::ENABLED))     return;
            if (!e) return;
            if (const auto* kp = e->getIf<sf::Event::KeyPressed>())
                handleKey(*kp);
        });

        onTextEntered([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (!checkFlag(ml::Flag::ENABLED))     return;
            if (checkFlag(Flag::READONLY))     return;
            if (!e) return;
            if (const auto* te = e->getIf<sf::Event::TextEntered>())
                handleChar(*te);
        });

        // ── Update: cursor blink + drag selection ─────────────────────────────
        onUpdate([this]{
            const bool focused = checkFlag(ml::Flag::FOCUSED);

            if (!focused)
                _cursorVisible = false;
            else if (_cursorClock.getElapsedTime().asSeconds() > 0.5f)
            {
                _cursorVisible = !_cursorVisible;
                _cursorClock.restart();
            }

            // Skip all mouse interaction when disabled via parent propagation.
            // TextInput polls raw mouse state directly, bypassing ClickableDispatcher,
            // so we must gate on ml::Flag::ENABLED explicitly here.
            if (!checkFlag(ml::Flag::ENABLED))
            {
                _prevMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                return;
            }

            const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            const sf::Vector2f wp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));
            const bool inBounds = getGlobalBounds().contains(wp);

            if (mouseDown && !_prevMouseDown && inBounds)
            {
                const std::size_t idx = hitTest(wp);
                const bool sameSpot   = std::hypot(wp.x - _lastClick.x,
                                                    wp.y - _lastClick.y) < 5.f;
                const bool quickClick = _clickClock.getElapsedTime().asMilliseconds() < 400;

                if (_waitingDouble && sameSpot && quickClick)
                {
                    _buffer.setCursor(idx);
                    _buffer.moveCursorByWord(-1, false);
                    const std::size_t ws = _buffer.getCursor();
                    _buffer.moveCursorByWord(+1, false);
                    _buffer.setSelection(ws, _buffer.getCursor());
                    _waitingDouble = false;
                    _dragging      = false;
                    rebuild();
                }
                else
                {
                    _waitingDouble = true;
                    _lastClick     = wp;
                    _clickClock.restart();
                    _dragAnchor    = idx;
                    _dragging      = true;
                }
            }
            else if (mouseDown && _dragging)
            {
                _buffer.setSelection(_dragAnchor, hitTest(wp));
                reflow();
            }
            else if (!mouseDown && _prevMouseDown && _dragging)
            {
                const std::size_t releaseIdx = hitTest(wp);
                if (releaseIdx != _dragAnchor)
                    _buffer.setSelection(_dragAnchor, releaseIdx);
                else
                    _buffer.setCursor(releaseIdx);
                _dragging = false;
                reflow();
            }

            _prevMouseDown = mouseDown;
        });
    }

    void TextInput::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        TextInputTheme::applyFrom(theme);
        _placeholder.setFillColor(placeholderColor);
        _placeholder.setCharacterSize(fontSize);
        _placeholder.setFont(*font);
        _background.setOutlineThickness(borderThickness);
        syncColors();
        rebuild();
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    void TextInput::rebuild()
    {
        _renderer.rebuild();

        if (passwordMode)
        {
            _maskDisplay.setFont(*font);
            _maskDisplay.setCharacterSize(fontSize);
            _maskDisplay.setFillColor(sf::Color::White);
            const std::u32string masked(
                _buffer.size(), static_cast<char32_t>(passwordChar));
            _maskDisplay.setString(sf::String::fromUtf32(masked.begin(), masked.end()));
        }

        reflow();
        onRebuildComplete();
    }

    void TextInput::reflow()
    {
        // Use the placeholder's actual glyph bounds to centre the text
        // vertically — this accounts for the font's ascender/descender gap
        // which differs from the raw fontSize integer.
        // Falls back to fontSize if the placeholder hasn't been measured yet.
        const sf::FloatRect lb = _placeholder.getLocalBounds();
        const float glyphH = lb.size.y > 0.f
                             ? lb.size.y
                             : static_cast<float>(fontSize);
        const float textY  = (size.y - glyphH) / 2.f - lb.position.y;
        _renderer.setOrigin({padding - _scrollX, textY});

        if (passwordMode)
            _maskDisplay.setPosition({padding - _scrollX, textY});

        syncPlaceholder();
    }

    void TextInput::rebuildAndScroll()
    {
        rebuild();

        const float cursorX      = _renderer.charIndexToPosition(_buffer.getCursor()).x;
        const float visibleLeft  = padding;
        const float visibleRight = size.x - padding;
        bool changed = false;

        if (cursorX < visibleLeft)
        {
            _scrollX = std::max(0.f, _scrollX - (visibleLeft - cursorX));
            changed  = true;
        }
        else if (cursorX > visibleRight)
        {
            _scrollX += cursorX - visibleRight;
            changed   = true;
        }

        if (_scrollX > 0.f)
        {
            const float textEnd = _renderer.charIndexToPosition(_buffer.size()).x;
            if (textEnd < visibleRight)
            {
                _scrollX = std::max(0.f, _scrollX - (visibleRight - textEnd));
                changed  = true;
            }
        }

        if (changed) reflow();
    }

    void TextInput::syncColors()
    {
        if (!checkFlag(ml::Flag::ENABLED))
        {
            _background.setFillColor(bgDisabled);
            _background.setOutlineColor(borderDisabled);
        }
        else if (isState(State::ERROR))
        {
            _background.setFillColor(bgIdle);
            _background.setOutlineColor(borderError);
        }
        else if (isState(State::FOCUSED))
        {
            _background.setFillColor(bgFocused);
            _background.setOutlineColor(borderFocused);
        }
        else
        {
            _background.setFillColor(bgIdle);
            _background.setOutlineColor(borderIdle);
        }
    }

    void TextInput::syncPlaceholder()
    {
        _showPlaceholder = _buffer.empty();
        const sf::FloatRect lb = _placeholder.getLocalBounds();
        _placeholder.setPosition({
            _position.x + padding,
            _position.y + (size.y - lb.size.y) / 2.f - lb.position.y
        });
    }

    std::size_t TextInput::hitTest(const sf::Vector2f& worldPos) const
    {
        return _renderer.positionToCharIndex({
            worldPos.x - _position.x + _scrollX,
            worldPos.y - _position.y
        });
    }

    void TextInput::handleKey(const sf::Event::KeyPressed& kp)
    {
        const bool shift = kp.shift;
        const bool ctrl  = kp.control || kp.system;

        switch (kp.code)
        {
            case sf::Keyboard::Key::Left:
                if (ctrl) _buffer.moveCursorByWord(-1, shift);
                else      _buffer.moveCursor(-1, shift);
                break;
            case sf::Keyboard::Key::Right:
                if (ctrl) _buffer.moveCursorByWord(+1, shift);
                else      _buffer.moveCursor(+1, shift);
                break;
            case sf::Keyboard::Key::Home:
                _buffer.moveCursorToLineStart(shift);
                _scrollX = 0.f;
                break;
            case sf::Keyboard::Key::End:
                _buffer.moveCursorToLineEnd(shift);
                break;
            case sf::Keyboard::Key::Backspace:
                if (!checkFlag(Flag::READONLY))
                {
                    _buffer.backspace();
                    if (_onChange) _onChange(_buffer.getText());
                }
                break;
            case sf::Keyboard::Key::Delete:
                if (!checkFlag(Flag::READONLY))
                {
                    _buffer.deleteForward();
                    if (_onChange) _onChange(_buffer.getText());
                }
                break;
            case sf::Keyboard::Key::Enter:
                if (_onSubmit) _onSubmit(_buffer.getText());
                break;
            case sf::Keyboard::Key::A:
                if (ctrl) _buffer.selectAll();
                break;
            case sf::Keyboard::Key::C:
                if (ctrl) _buffer.copyToClipboard();
                break;
            case sf::Keyboard::Key::X:
                if (ctrl && !checkFlag(Flag::READONLY))
                {
                    _buffer.cutToClipboard();
                    if (_onChange) _onChange(_buffer.getText());
                }
                break;
            case sf::Keyboard::Key::V:
                if (ctrl && !checkFlag(Flag::READONLY))
                {
                    _buffer.pasteFromClipboard();
                    if (_onChange) _onChange(_buffer.getText());
                }
                break;
            default: break;
        }

        rebuildAndScroll();
        _cursorClock.restart();
        _cursorVisible = true;
    }

    void TextInput::handleChar(const sf::Event::TextEntered& te)
    {
        if (te.unicode < 32 || te.unicode == 127) return;
        if (maxLength > 0 && _buffer.size() >= maxLength) return;
        _buffer.insertChar(te.unicode);
        if (_onChange) _onChange(_buffer.getText());
        rebuildAndScroll();
        _cursorClock.restart();
        _cursorVisible = true;
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void TextInput::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_background, states);

        if (_showPlaceholder)
            target.draw(_placeholder, states);

        _canvas.clear(sf::Color::Transparent);
        sf::RenderStates cs;

        if (!_showPlaceholder)
        {
            if (passwordMode)
            {
                _canvas.draw(_maskDisplay, cs);
            }
            else
            {
                if (_buffer.hasSelection())
                    _renderer.drawSelection(_canvas, cs,
                        _buffer.getSelectionStart(),
                        _buffer.getSelectionEnd(),
                        selectionColor);
                _renderer.draw(_canvas, cs);
            }
        }

        if (_cursorVisible
            && checkFlag(ml::Flag::FOCUSED)
            && checkFlag(ml::Flag::ENABLED)
            && !_dragging)
        {
            _renderer.drawCursor(_canvas, cs, _buffer.getCursor(), cursorColor);
        }
        _canvas.display();
        sf::Sprite sprite(_canvas.getTexture());
        sprite.setPosition(_position);
        target.draw(sprite, states);
    }

    // ── Value ─────────────────────────────────────────────────────────────────

    void TextInput::setValue(const std::string& value)
    {
        _buffer.setText(value);
        _scrollX = 0.f;
        rebuild();
        if (_onChange) _onChange(value);
    }

    std::string TextInput::getValue() const { return _buffer.getText(); }

    std::string TextInput::getRichText() const
    {
        ml::json j;
        j["text"]  = _buffer.getText();
        j["spans"] = ml::json::array();
        for (const auto& a : _buffer.getAttributes())
        {
            ml::json s;
            s["start"] = a.start;
            s["end"]   = a.end;
            if (a.bold.has_value())        s["bold"]      = *a.bold;
            if (a.italic.has_value())      s["italic"]    = *a.italic;
            if (a.underline.has_value())   s["underline"] = *a.underline;
            if (a.charSize.has_value())    s["size"]      = *a.charSize;
            if (a.align.has_value())       s["align"]     = *a.align;        // 0/1/2
            if (a.listType.has_value())    s["list"]      = *a.listType;     // 0/1/2
            if (a.indentLevel.has_value()) s["indent"]    = *a.indentLevel;
            if (a.font && a.font == _monoFont) s["mono"]  = true;            // code runs
            if (a.color.has_value())
                s["color"] = { {"r", a.color->r}, {"g", a.color->g},
                               {"b", a.color->b}, {"a", a.color->a} };
            j["spans"].push_back(s);
        }
        return j.dump();
    }

    void TextInput::setRichText(const std::string& jsonStr)
    {
        const ml::json j = ml::json::parse(jsonStr, nullptr, /*allow_exceptions=*/false);
        if (j.is_discarded() || !j.is_object())
        {
            // Not valid rich JSON — treat the input as plain text.
            setValue(jsonStr);
            return;
        }

        _buffer.setText(j.value("text", std::string{}));
        if (j.contains("spans") && j["spans"].is_array())
        {
            for (const auto& s : j["spans"])
            {
                TextAttribute a;
                const std::size_t start = s.value("start", std::size_t{0});
                const std::size_t end   = s.value("end",   std::size_t{0});
                if (s.contains("bold"))      a.bold      = s["bold"].get<bool>();
                if (s.contains("italic"))    a.italic    = s["italic"].get<bool>();
                if (s.contains("underline")) a.underline = s["underline"].get<bool>();
                if (s.contains("size"))      a.charSize  = s["size"].get<unsigned int>();
                if (s.contains("align"))     a.align       = s["align"].get<int>();
                if (s.contains("list"))      a.listType    = s["list"].get<int>();
                if (s.contains("indent"))    a.indentLevel = s["indent"].get<int>();
                if (s.value("mono", false) && _monoFont) a.font = _monoFont;
                if (s.contains("color"))
                {
                    const auto& c = s["color"];
                    a.color = sf::Color(
                        static_cast<std::uint8_t>(c.value("r", 0)),
                        static_cast<std::uint8_t>(c.value("g", 0)),
                        static_cast<std::uint8_t>(c.value("b", 0)),
                        static_cast<std::uint8_t>(c.value("a", 255)));
                }
                _buffer.applyAttributeRange(start, end, a);
            }
        }
        _scrollX = 0.f;
        rebuild();
        if (_onChange) _onChange(_buffer.getText());
    }

    void TextInput::clear()
    {
        _buffer.clear();
        _scrollX = 0.f;
        rebuild();
        if (_onChange) _onChange("");
    }

    // ── Rich text selection styling ───────────────────────────────────────────

    void TextInput::setSelectionFont(const sf::Font& f)
    { TextAttribute a; a.font = &f;    _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionCharSize(unsigned int s)
    { TextAttribute a; a.charSize = s; _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionColor(const sf::Color& c)
    { TextAttribute a; a.color = c;    _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionBold(bool b)
    { TextAttribute a; a.bold = b;     _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionItalic(bool i)
    { TextAttribute a; a.italic = i;   _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionUnderline(bool u)
    { TextAttribute a; a.underline = u; _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionAlign(int align)
    { TextAttribute a; a.align = align; _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionListType(int listType)
    { TextAttribute a; a.listType = listType; _buffer.applyAttribute(a); rebuild(); }

    void TextInput::changeSelectionIndent(int delta)
    {
        // Indent is paragraph-level: expand to the enclosing paragraph(s), read
        // the current depth, then apply the clamped new depth.
        const std::string& text = _buffer.getText();
        std::size_t a = _buffer.getSelectionStart();
        std::size_t b = _buffer.getSelectionEnd();
        while (a > 0 && text[a - 1] != '\n') --a;
        while (b < text.size() && text[b] != '\n') ++b;

        // Read the current depth. An empty paragraph carries no char, so probe the
        // preceding newline; and if a pending indent is already staged (repeated
        // Tab before typing), build on that so Tab keeps nesting deeper.
        int cur;
        if (a == b && _buffer.hasPendingAttribute()
            && _buffer.getPendingAttribute().indentLevel.has_value())
        {
            cur = _buffer.getPendingAttribute().indentLevel.value();
        }
        else
        {
            const std::size_t probe = (a < text.size()) ? a : (a > 0 ? a - 1 : a);
            cur = _buffer.getAttributeAt(
                probe, nullptr, getCharacterSize(), _renderer.getDefaultColor())
                    .indentLevel.value_or(0);
        }
        const int next = std::max(0, cur + delta);

        const std::size_t caret = _buffer.getCursor();
        TextAttribute attr; attr.indentLevel = next;
        if (a == b) {                       // empty paragraph — style the next typing
            _buffer.setCursor(a);
            _buffer.applyAttribute(attr);
        } else {
            _buffer.setSelection(a, b);
            _buffer.applyAttribute(attr);
            _buffer.setCursor(caret);
        }
        rebuild();
    }

    void TextInput::selectAll()
    { _buffer.selectAll(); rebuild(); }

    std::size_t TextInput::getSelectionStart() const { return _buffer.getSelectionStart(); }
    std::size_t TextInput::getSelectionEnd()   const { return _buffer.getSelectionEnd(); }
    std::size_t TextInput::getCursor()         const { return _buffer.getCursor(); }

    const sf::Font* TextInput::getFontAt(std::size_t index) const
    {
        // nullptr default font → returns the styled font if one is set, else nullptr.
        return _buffer.getAttributeAt(index, nullptr,
                                      getCharacterSize(), _renderer.getDefaultColor()).font;
    }

    sf::Color TextInput::getColorAt(std::size_t index) const
    {
        return _buffer.getAttributeAt(index, nullptr, getCharacterSize(),
                                      _renderer.getDefaultColor())
            .color.value_or(_renderer.getDefaultColor());
    }

    unsigned int TextInput::getCharSizeAt(std::size_t index) const
    {
        return _buffer.getAttributeAt(index, nullptr, getCharacterSize(),
                                      _renderer.getDefaultColor())
            .charSize.value_or(getCharacterSize());
    }

    TextAttribute TextInput::getActiveStyle() const
    {
        const std::size_t idx = _buffer.hasSelection()
            ? _buffer.getSelectionStart()
            : (_buffer.getCursor() > 0 ? _buffer.getCursor() - 1 : _buffer.getCursor());

        TextAttribute a = _buffer.getAttributeAt(
            idx, nullptr, getCharacterSize(), _renderer.getDefaultColor());

        // With no selection, the next-typed style is the pending overlay (set by a
        // toolbar toggle while nothing is selected) on top of the inherited run.
        if (!_buffer.hasSelection() && _buffer.hasPendingAttribute())
        {
            const TextAttribute& p = _buffer.getPendingAttribute();
            if (p.font)                   a.font        = p.font;
            if (p.charSize.has_value())   a.charSize    = p.charSize;
            if (p.color.has_value())      a.color       = p.color;
            if (p.bold.has_value())       a.bold        = p.bold;
            if (p.italic.has_value())     a.italic      = p.italic;
            if (p.underline.has_value())  a.underline   = p.underline;
            if (p.align.has_value())      a.align       = p.align;
            if (p.listType.has_value())   a.listType    = p.listType;
            if (p.indentLevel.has_value())a.indentLevel = p.indentLevel;
        }
        return a;
    }

    sf::Color TextInput::getTextColor() const { return _renderer.getDefaultColor(); }

    void TextInput::setClearSelectionOnBlur(bool enabled) { _clearSelectionOnBlur = enabled; }

    void TextInput::setSelection(std::size_t s, std::size_t e)
    { _buffer.setSelection(s, e); rebuild(); }

    std::string TextInput::getSelectedText() const
    { return _buffer.getSelectedText(); }

    // ── Placeholder ───────────────────────────────────────────────────────────

    void TextInput::setPlaceholder(const std::string& t)
    { _placeholder.setString(t); syncPlaceholder(); }

    std::string TextInput::getPlaceholder() const
    { return _placeholder.getString(); }

    // ── Enabled / disabled / readonly ─────────────────────────────────────────

    void TextInput::onEnabledChanged(bool /*enabled*/)
    {
        // Disabled colors derive from Flag::ENABLED; refresh on both paths and
        // leave State alone to preserve FOCUSED/ERROR.
        syncColors();
    }

    void TextInput::setReadOnly(bool r)
    { if (r) enableFlag(Flag::READONLY); else disableFlag(Flag::READONLY); }

    bool TextInput::isReadOnly() const { return checkFlag(Flag::READONLY); }

    // ── Error state ───────────────────────────────────────────────────────────

    void TextInput::setError(bool error)
    {
        setState(error ? State::ERROR
                       : (isState(State::FOCUSED) ? State::FOCUSED : State::IDLE));
        syncColors();
    }

    bool TextInput::hasError() const { return isState(State::ERROR); }

    // ── Size / font ───────────────────────────────────────────────────────────

    void TextInput::setSize(const sf::Vector2f& sz)
    {
        size = sz;
        _background.setSize(sz);
        static_cast<void>(_canvas.resize({
            std::max(1u, static_cast<unsigned int>(sz.x)),
            std::max(1u, static_cast<unsigned int>(sz.y))
        }));
        rebuild();
    }

    sf::Vector2f TextInput::getSize() const { return size; }

    void TextInput::setFont(const sf::Font& f)
    {
        font = &f;
        _placeholder.setFont(f);
        FontManager<>::prewarm(f, fontSize);
        rebuild();
    }

    void TextInput::setCharacterSize(unsigned int sz)
    {
        fontSize = sz;
        _placeholder.setCharacterSize(sz);
        FontManager<>::prewarm(*font, sz);
        rebuild();
    }

    unsigned int TextInput::getCharacterSize() const { return fontSize; }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void TextInput::onChange(std::function<void(const std::string&)> cb)
    { _onChange = std::move(cb); }

    void TextInput::onSubmit(std::function<void(const std::string&)> cb)
    { _onSubmit = std::move(cb); }

    // ── Positionable ──────────────────────────────────────────────────────────

    void TextInput::setPosition(const sf::Vector2f& pos)
    {
        _position = {std::round(pos.x), std::round(pos.y)};
        _background.setPosition(_position);
        reflow();
    }

    sf::Vector2f  TextInput::getPosition()     const { return _position; }
    sf::FloatRect TextInput::getGlobalBounds() const { return _background.getGlobalBounds(); }

} // namespace ml
