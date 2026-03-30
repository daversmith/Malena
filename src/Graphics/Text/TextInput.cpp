// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    TextInput::TextInput(const sf::Font& font)
        : _font(&font),
          _placeholder(font),
          _renderer(_buffer, font, 16, sf::Color::White)
    {
    	for (unsigned int size : {11u, 12u, 14u, 16u, 18u, 20u, 24u})
    		ml::FontManager<>::prewarm(font, size);
        _background.setSize(_size);
        _background.setFillColor(_bgIdle);
        _background.setOutlineColor(_borderIdle);
        _background.setOutlineThickness(_borderThick);

        _placeholder.setCharacterSize(_charSize);
    	FontManager<>::prewarm(font, _charSize);
        _placeholder.setFillColor(_phColor);

        _canvas.resize({
            static_cast<unsigned int>(_size.x),
            static_cast<unsigned int>(_size.y)
        });

        rebuild();

        // ── Focus / blur ──────────────────────────────────────────────────────
        onFocus([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::FOCUSED);
            syncColors();
            _cursorClock.restart();
            _cursorVisible = true;
        });

        onBlur([this]{std::cout << "blur fired, this=" << this << "\n";
            _dragging      = false;
            _waitingDouble = false;
            // DO NOT reset _prevMouseDown here — resetting it causes the
            // blur/focus cycle on the same field to re-fire a press every frame.

            const sf::Vector2f mp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));

            if (!getGlobalBounds().contains(mp))
            {
                _buffer.clearSelection();
                reflow();   // selection cleared — no content change, reflow only
            }

            if (!isState(State::ERROR)) setState(State::IDLE);
            syncColors();
        });

        // ── Keyboard ──────────────────────────────────────────────────────────
        // Use checkFlag(ml::Flag::FOCUSED) — the system flag, proven reliable.
        onKeypress([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (checkFlag(Flag::DISABLED))     return;
            if (!e) return;
            if (const auto* kp = e->getIf<sf::Event::KeyPressed>())
                handleKey(*kp);
        });

        onTextEntered([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (checkFlag(Flag::DISABLED))     return;
            if (checkFlag(Flag::READONLY))     return;
            if (!e) return;
            if (const auto* te = e->getIf<sf::Event::TextEntered>())
                handleChar(*te);
        });

        // ── Update: cursor blink + drag selection (polling) ───────────────────
        //
        // All drag selection is handled here via sf::Mouse::isButtonPressed
        // polling. This eliminates all event-ordering races between
        // onMousePressed, onMouseMoved, onMouseReleased, and onClick.
        //
        // Edge detection (_prevMouseDown) reliably detects press-start and
        // release transitions every frame.
        onUpdate([this]{
            const bool focused = checkFlag(ml::Flag::FOCUSED);

            // ── Cursor blink ──────────────────────────────────────────────────
            if (!focused)
            {
                _cursorVisible = false;
                // Do NOT reset _prevMouseDown here — it must only be updated
                // at the bottom of this lambda to correctly track transitions.
            }
            else if (_cursorClock.getElapsedTime().asSeconds() > 0.5f)
            {
                _cursorVisible = !_cursorVisible;
                _cursorClock.restart();
            }

            // ── Drag selection ────────────────────────────────────────────────
            const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            const sf::Vector2f wp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));
            const bool inBounds = getGlobalBounds().contains(wp);

            // Press started this frame
            if (mouseDown && !_prevMouseDown && inBounds)
            {
                const std::size_t idx = hitTest(wp);

                // Double-click detection
                const bool sameSpot  = std::hypot(wp.x - _lastClick.x,
                                                   wp.y - _lastClick.y) < 5.f;
                const bool quickClick = _clickClock.getElapsedTime().asMilliseconds() < 400;

                if (_waitingDouble && sameSpot && quickClick)
                {
                    // Select word
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
            // Mouse held — update live selection
            else if (mouseDown && _dragging)
            {
                const std::size_t idx = hitTest(wp);
                _buffer.setSelection(_dragAnchor, idx);
                reflow();   // selection change only — no sf::Text rebuild needed
            }
            // Mouse released this frame
            else if (!mouseDown && _prevMouseDown && _dragging)
            {
                const std::size_t releaseIdx = hitTest(wp);

                if (releaseIdx != _dragAnchor)
                    _buffer.setSelection(_dragAnchor, releaseIdx);
                else
                    _buffer.setCursor(releaseIdx);

                _dragging = false;
                reflow();   // selection change only — no sf::Text rebuild needed
            }

            _prevMouseDown = mouseDown;
        });
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    void TextInput::rebuild()
    {
        // Full rebuild — recreates sf::Text objects from buffer content.
        // Call this when buffer content or styling changes.
        _renderer.rebuild();
        reflow();
    }

    void TextInput::reflow()
    {
        // Reposition only — never recreates sf::Text objects.
        // Origin is canvas-local (relative to {0,0}) so glyphs always
        // rasterize at integer pixel positions, preventing atlas corruption.
        const float textY = (_size.y - static_cast<float>(_charSize)) / 2.f;
        _renderer.setOrigin({_padding - _scrollX, textY});
        syncPlaceholder();
    }

    void TextInput::rebuildAndScroll()
    {
        // Step 1 — rebuild sf::Text objects (content changed)
        rebuild();

        // Step 2 — adjust scroll using canvas-local coordinates
        const float cursorX      = _renderer.charIndexToPosition(_buffer.getCursor()).x;
        const float visibleLeft  = _padding;
        const float visibleRight = _size.x - _padding;
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

        // Step 3 — reflow only (reposition, no new sf::Text objects)
        if (changed) reflow();
    }

    void TextInput::syncColors()
    {
        if (checkFlag(Flag::DISABLED))
        {
            _background.setFillColor(_bgDisabled);
            _background.setOutlineColor(_borderDis);
        }
        else if (isState(State::ERROR))
        {
            _background.setFillColor(_bgIdle);
            _background.setOutlineColor(_borderError);
        }
        else if (isState(State::FOCUSED))
        {
            _background.setFillColor(_bgFocused);
            _background.setOutlineColor(_borderFocus);
        }
        else
        {
            _background.setFillColor(_bgIdle);
            _background.setOutlineColor(_borderIdle);
        }
    }

    void TextInput::syncPlaceholder()
    {
        _showPlaceholder = _buffer.empty();
        const sf::FloatRect lb = _placeholder.getLocalBounds();
        _placeholder.setPosition({
            _position.x + _padding,
            _position.y + (_size.y - lb.size.y) / 2.f - lb.position.y
        });
    }

    std::size_t TextInput::hitTest(const sf::Vector2f& worldPos) const
    {
        // Convert world pos to canvas-local space (renderer lives at {0,0})
        return _renderer.positionToCharIndex({
            worldPos.x - _position.x + _scrollX,
            worldPos.y - _position.y
        });
    }

    // ── handleKey ─────────────────────────────────────────────────────────────

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
        if (_maxLength > 0 && _buffer.size() >= _maxLength) return;

        _buffer.insertChar(te.unicode);
        if (_onChange) _onChange(_buffer.getText());
        rebuildAndScroll();
        _cursorClock.restart();
        _cursorVisible = true;
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void TextInput::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Background drawn to main target in world space
        target.draw(_background, states);

        // Placeholder drawn directly — no clipping needed, fits by design
        if (_showPlaceholder)
            target.draw(_placeholder, states);

        // Text, selection, and cursor drawn into canvas (canvas-local space).
        // No transform needed — renderer origin is already at {0,0}-relative.
        // Canvas then blitted to world space via sprite, providing clean clipping.
        _canvas.clear(sf::Color::Transparent);

        sf::RenderStates cs; // identity — renderer is in canvas-local space

        if (!_showPlaceholder)
        {
            if (_buffer.hasSelection())
                _renderer.drawSelection(_canvas, cs,
                    _buffer.getSelectionStart(),
                    _buffer.getSelectionEnd(),
                    _selColor);

            _renderer.draw(_canvas, cs);
        }

        // Cursor hidden while dragging so it doesn't follow the selection
        if (_cursorVisible
            && checkFlag(ml::Flag::FOCUSED)
            && !checkFlag(Flag::DISABLED)
            && !_dragging)
        {
            _renderer.drawCursor(_canvas, cs, _buffer.getCursor(), _cursorColor);
        }

        _canvas.display();

        sf::Sprite sprite(_canvas.getTexture());
        sprite.setPosition(_position);  // world-space position applied here only
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

    void TextInput::clear()
    {
        _buffer.clear();
        _scrollX = 0.f;
        rebuild();
        if (_onChange) _onChange("");
    }

    // ── Rich text styling ─────────────────────────────────────────────────────

    void TextInput::setSelectionFont(const sf::Font& f)
    { TextAttribute a; a.font = &f;   _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionCharSize(unsigned int s)
    { TextAttribute a; a.charSize = s; _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionColor(const sf::Color& c)
    { TextAttribute a; a.color = c;   _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionBold(bool b)
    { TextAttribute a; a.bold = b;    _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionItalic(bool i)
    { TextAttribute a; a.italic = i;  _buffer.applyAttribute(a); rebuild(); }

    void TextInput::setSelectionUnderline(bool u)
    { TextAttribute a; a.underline = u; _buffer.applyAttribute(a); rebuild(); }

    // ── Selection ─────────────────────────────────────────────────────────────

    void TextInput::selectAll()
    { _buffer.selectAll(); rebuild(); }

    void TextInput::setSelection(std::size_t s, std::size_t e)
    { _buffer.setSelection(s, e); rebuild(); }

    std::string TextInput::getSelectedText() const
    { return _buffer.getSelectedText(); }

    // ── Options ───────────────────────────────────────────────────────────────

    void TextInput::setMaxLength(std::size_t l) { _maxLength = l; }
    std::size_t TextInput::getMaxLength() const { return _maxLength; }
    void TextInput::setPasswordMode(bool e)     { _passwordMode = e; }
    bool TextInput::isPasswordMode() const      { return _passwordMode; }
    void TextInput::setPasswordChar(char32_t c) { _passwordChar = c; }

    // ── Placeholder ───────────────────────────────────────────────────────────

    void TextInput::setPlaceholder(const std::string& t)
    { _placeholder.setString(t); syncPlaceholder(); }

    std::string TextInput::getPlaceholder() const
    { return _placeholder.getString(); }

    // ── Enabled / disabled / readonly ─────────────────────────────────────────

    void TextInput::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        syncColors();
    }

    bool TextInput::isEnabled()  const { return !checkFlag(Flag::DISABLED); }

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

    // ── Styling ───────────────────────────────────────────────────────────────

    void TextInput::setSize(const sf::Vector2f& size)
    {
        _size = size;
        _background.setSize(size);
        _canvas.resize({
            static_cast<unsigned int>(size.x),
            static_cast<unsigned int>(size.y)
        });
        rebuild();
    }

    sf::Vector2f TextInput::getSize() const { return _size; }

    void TextInput::setBackgroundColor(const sf::Color& c)        { _bgIdle      = c; syncColors(); }
    void TextInput::setBackgroundFocusedColor(const sf::Color& c) { _bgFocused   = c; syncColors(); }
    void TextInput::setBorderColor(const sf::Color& c)            { _borderIdle  = c; syncColors(); }
    void TextInput::setBorderFocusedColor(const sf::Color& c)     { _borderFocus = c; syncColors(); }
    void TextInput::setBorderErrorColor(const sf::Color& c)       { _borderError = c; syncColors(); }
    void TextInput::setSelectionHighlightColor(const sf::Color& c){ _selColor    = c; }
    void TextInput::setCursorColor(const sf::Color& c)            { _cursorColor = c; }

    void TextInput::setBorderThickness(float t)
    { _borderThick = t; _background.setOutlineThickness(t); }

    void TextInput::setDefaultTextColor(const sf::Color& c)
    { _textColor = c; rebuild(); }

    void TextInput::setPlaceholderColor(const sf::Color& c)
    { _phColor = c; _placeholder.setFillColor(c); }

    void TextInput::setPadding(float p) { _padding = p; rebuild(); }
    float TextInput::getPadding() const { return _padding; }

    // ── Font ──────────────────────────────────────────────────────────────────

    void TextInput::setFont(const sf::Font& font)
    {
	    _font = &font;
    	_placeholder.setFont(font);
    	FontManager<>::prewarm(font, _charSize);
    	rebuild();
    }

    void TextInput::setCharacterSize(unsigned int size)
    {
	    _charSize = size;
    	_placeholder.setCharacterSize(size);
    	FontManager<>::prewarm(*_font, _charSize);
    	rebuild();
    }

    unsigned int TextInput::getCharacterSize() const { return _charSize; }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void TextInput::onChange(std::function<void(const std::string&)> cb) { _onChange = std::move(cb); }
    void TextInput::onSubmit(std::function<void(const std::string&)> cb) { _onSubmit = std::move(cb); }

    // ── Positionable ──────────────────────────────────────────────────────────

    // void TextInput::setPosition(const sf::Vector2f& pos)
    // {
    //     _position = pos;
    //     _background.setPosition(pos);
    //     reflow();   // position changed — reflow only, no new sf::Text objects
    // }
	void TextInput::setPosition(const sf::Vector2f& pos)
    {
    	_position = {std::round(pos.x), std::round(pos.y)};
    	_background.setPosition(_position);
    	reflow();
    }

    sf::Vector2f  TextInput::getPosition()     const { return _position; }
    sf::FloatRect TextInput::getGlobalBounds() const { return _background.getGlobalBounds(); }

} // namespace ml