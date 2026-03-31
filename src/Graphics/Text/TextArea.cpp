// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/TextArea.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>

namespace ml
{
    TextArea::TextArea(const sf::Font& font_)
        : TextInput(font_),
          _scrollPane(size.x, size.y)
    {
        // Seed TextAreaTheme from active theme then apply constructor font
        TextAreaTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        // ── ScrollPane setup ──────────────────────────────────────────────────
        // Transparent background — TextInput draws the field background.
        // ScrollPane contributes only the scrollbar.
        _scrollPane.setBackgroundColor(sf::Color::Transparent);
        _scrollPane.setScrollBarColor(scrollBarColor);
        _scrollPane.setScrollBarWidth(scrollBarWidth);
        _scrollPane.setPosition(_position);

        // Silence ScrollPane AND its internal scrollbar thumb from the
        // event system. Both are Components with auto-subscriptions.
        // Without this, clicking TextArea also hits the thumb (same bounds),
        // causing it to steal focus and clear the FOCUSED flag.
        _scrollPane.embed();

        // ── Multiline key handling ────────────────────────────────────────────
        onKeypress([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (checkFlag(Flag::DISABLED))     return;
            if (!e) return;
            if (const auto* kp = e->getIf<sf::Event::KeyPressed>())
                handleTextAreaKeypress(*kp);
        });

        // ── Scroll wheel ──────────────────────────────────────────────────────
        // ScrollPane's own onScroll is silenced. We handle the wheel here and
        // drive ScrollPane via setScrollOffsetY so it stays as the single
        // source of scroll truth.
        onScroll([this](const std::optional<sf::Event>& e){
            if (!e) return;
            const auto* scroll = e->getIf<sf::Event::MouseWheelScrolled>();
            if (!scroll) return;
            const float contentH  = _renderer.getTotalHeight() + padding * 2.f;
            const float maxScroll = std::max(0.f, contentH - size.y);
            const float newY = std::clamp(
                _scrollPane.getScrollOffsetY()
                    - scroll->delta * static_cast<float>(fontSize),
                0.f, maxScroll);
            _scrollPane.setScrollOffsetY(newY);
            reflow();
        });

        // ── Per-frame: scroll sync + scrollbar thumb drag ─────────────────────
        onUpdate([this]{
            // Sync renderer when scroll offset changes (wheel events)
            const float scrollY = _scrollPane.getScrollOffsetY();
            if (scrollY != _prevScrollY)
            {
                _prevScrollY = scrollY;
                reflow();
            }

            // ── Scrollbar thumb drag ──────────────────────────────────────────
            // ScrollPane's own drag is disabled via embed(). TextArea handles
            // thumb dragging here using the same geometry as drawScrollbar().
            const float contentH = std::max(size.y + 1.f,
                _renderer.getTotalHeight() + padding * 2.f);
            if (contentH <= size.y) { _thumbDragging = false; _prevMouseDown = false; return; }

            const float maxScroll = contentH - size.y;
            const float ratio     = size.y / contentH;
            const float thumbH    = std::max(20.f, size.y * ratio);
            const float trackX    = _position.x + size.x - scrollBarWidth;
            const float curScroll = _scrollPane.getScrollOffsetY();
            const float thumbTop  = _position.y +
                (maxScroll > 0.f ? (curScroll / maxScroll) * (size.y - thumbH) : 0.f);

            const sf::FloatRect thumbRect({trackX, thumbTop}, {scrollBarWidth, thumbH});

            const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            if (mouseDown && !_prevMouseDown && thumbRect.contains(wp))
            {
                _thumbDragging        = true;
                _thumbDragStartMouseY = wp.y;
                _thumbDragStartScroll = curScroll;
            }
            else if (mouseDown && _thumbDragging)
            {
                const float delta    = wp.y - _thumbDragStartMouseY;
                const float trackH   = size.y - thumbH;
                const float newScroll = trackH > 0.f
                    ? _thumbDragStartScroll + delta * maxScroll / trackH
                    : 0.f;
                _scrollPane.setScrollOffsetY(newScroll);
                reflow();
            }
            else if (!mouseDown)
            {
                _thumbDragging = false;
            }

            // Suppress text selection while scrollbar thumb is being dragged
            if (_thumbDragging)
            {
                _dragging = false;
                _buffer.clearSelection();
            }

            _prevMouseDown = mouseDown;
        });

        // Enable word wrap — reserve scrollBarWidth on the right for the scrollbar
        _renderer.setMaxWidth(size.x - padding * 2.f - scrollBarWidth);

        // Fix initial origin — TextInput's ctor fired TextInput::reflow()
        // (vertically centred) before the vtable was set to TextArea.
        // Now the vtable is correct, so this call sets the top-aligned origin.
        reflow();
    }

    // ── onRebuildComplete ─────────────────────────────────────────────────────

    void TextArea::onRebuildComplete()
    {
        // Canvas stays at the VISIBLE size — scrolling is achieved by shifting
        // the renderer origin in reflow(), identical to TextInput's horizontal
        // scroll. Content outside the canvas bounds is clipped by the RenderTexture.
        _canvas.resize({
            static_cast<unsigned int>(size.x),
            static_cast<unsigned int>(size.y)
        });

        // Tell ScrollPane the TOTAL content height for scrollbar calibration.
        const float contentH = std::max(size.y,
            _renderer.getTotalHeight() + padding * 2.f);
        _scrollPane.setContentHeight(contentH);
    }

    // ── reflow ────────────────────────────────────────────────────────────────

    void TextArea::reflow()
    {
        // Top-aligned. ScrollPane owns the vertical scroll state.
        const float scrollY = _scrollPane.getScrollOffsetY();
        _renderer.setOrigin({padding - _scrollX, padding - scrollY});
        syncPlaceholder();
    }

    void TextArea::syncPlaceholder()
    {
        _showPlaceholder = _buffer.empty();
        const sf::FloatRect lb = _placeholder.getLocalBounds();
        _placeholder.setPosition({
            _position.x + padding,
            _position.y + padding - lb.position.y
        });
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void TextArea::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        TextInputTheme::applyFrom(theme);
        TextAreaTheme::applyFrom(theme);
        _placeholder.setFillColor(placeholderColor);
        _placeholder.setCharacterSize(fontSize);
        _placeholder.setFont(*font);
        _background.setOutlineThickness(borderThickness);
        _scrollPane.setScrollBarColor(scrollBarColor);
        _scrollPane.setBackgroundColor(scrollBarTrackColor);
        _scrollPane.setScrollBarWidth(scrollBarWidth);
        syncColors();
        rebuild();
    }

    // ── handleTextAreaKeypress ────────────────────────────────────────────────

    void TextArea::handleTextAreaKeypress(const sf::Event::KeyPressed& kp)
    {
        const bool ctrl = kp.control || kp.system;

        if (kp.code == sf::Keyboard::Key::Enter && ctrl)
        {
            if (_onSubmit) _onSubmit(_buffer.getText());
            return;
        }

        if (kp.code == sf::Keyboard::Key::Enter && !ctrl)
        {
            if (!checkFlag(Flag::READONLY))
            {
                _buffer.insertChar(U'\n');
                if (_onChange) _onChange(_buffer.getText());
                rebuildAndScroll();
                _cursorClock.restart();
                _cursorVisible = true;
            }
            return;
        }

        if (kp.code == sf::Keyboard::Key::Up)
        {
            const std::size_t newIdx = _renderer.charIndexAbove(_buffer.getCursor());
            if (kp.shift) _buffer.setSelection(_buffer.getCursor(), newIdx);
            else          _buffer.setCursor(newIdx);
            rebuildAndScroll();
            _cursorClock.restart();
            _cursorVisible = true;
            return;
        }

        if (kp.code == sf::Keyboard::Key::Down)
        {
            const std::size_t newIdx = _renderer.charIndexBelow(_buffer.getCursor());
            if (kp.shift) _buffer.setSelection(_buffer.getCursor(), newIdx);
            else          _buffer.setCursor(newIdx);
            rebuildAndScroll();
            _cursorClock.restart();
            _cursorVisible = true;
            return;
        }
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void TextArea::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Field background (solid rect, clipped at field bounds)
        target.draw(_background, states);

        // Placeholder
        if (_showPlaceholder)
            target.draw(_placeholder, states);

        // ── Text, selection and cursor into the (tall) canvas ─────────────────
        // The canvas is sized to the full content height, so all lines exist.
        // ScrollPane's RenderTexture view clips the visible portion when it
        // draws the canvas as a child — except we're not adding the canvas as
        // a child. Instead we draw the canvas sprite offset by scroll, and
        // the field background acts as the clip boundary (drawn first so it
        // shows through on the sides, drawn second as a mask via draw order).
        _canvas.clear(sf::Color::Transparent);
        sf::RenderStates cs;

        if (!_showPlaceholder)
        {
            if (_buffer.hasSelection())
                _renderer.drawSelection(_canvas, cs,
                    _buffer.getSelectionStart(),
                    _buffer.getSelectionEnd(),
                    selectionColor);
            _renderer.draw(_canvas, cs);
        }

        if (_cursorVisible
            && checkFlag(ml::Flag::FOCUSED)
            && !checkFlag(Flag::DISABLED)
            && !_dragging)
        {
            _renderer.drawCursor(_canvas, cs, _buffer.getCursor(), cursorColor);
        }

        _canvas.display();

        // Sprite drawn at position, vertically offset by current scroll —
        // only the field-sized region is visible because the background was
        // already drawn behind it and the sprite sits over that area.
        // Sprite always at _position — the scroll effect comes from the
        // renderer origin shift in reflow(), which moves content upward
        // within the fixed-size canvas as scrollY increases.
        sf::Sprite sprite(_canvas.getTexture());
        sprite.setPosition(_position);
        target.draw(sprite, states);

        // Draw scrollbar directly — bypasses ScrollPane's RenderTexture
        // which erases underlying canvas pixels due to SFML BlendAlpha
        // compositing behaviour when using transparent RenderTexture sprites.
        drawScrollbar(target, states);
    }

    // ── drawScrollbar ─────────────────────────────────────────────────────────

    void TextArea::drawScrollbar(sf::RenderTarget& target,
                                 const sf::RenderStates& states) const
    {
        // Compute content vs visible metrics from ScrollPane state
        const float contentH  = _scrollPane.getScrollOffsetY() > 0.f
                                ? _renderer.getTotalHeight() + padding * 2.f
                                : std::max(size.y, _renderer.getTotalHeight() + padding * 2.f);
        const float visible   = size.y;

        if (contentH <= visible) return; // no scrollbar needed

        const float maxScroll = contentH - visible;
        const float scrollY   = _scrollPane.getScrollOffsetY();
        const float ratio     = visible / contentH;
        const float thumbH    = std::max(20.f, visible * ratio);
        const float thumbY    = (maxScroll > 0.f)
                                ? (scrollY / maxScroll) * (visible - thumbH)
                                : 0.f;
        const float trackX    = _position.x + size.x - scrollBarWidth;

        // Track
        sf::RectangleShape track({scrollBarWidth, visible});
        track.setPosition({trackX, _position.y});
        track.setFillColor(scrollBarTrackColor);
        target.draw(track, states);

        // Thumb
        sf::RectangleShape thumb({scrollBarWidth, thumbH});
        thumb.setPosition({trackX, _position.y + thumbY});
        thumb.setFillColor(scrollBarColor);
        target.draw(thumb, states);
    }

        // ── Scrollbar styling ─────────────────────────────────────────────────────

    void TextArea::setScrollBarColor(const sf::Color& c)
    {
        scrollBarColor = c;
        _scrollPane.setScrollBarColor(c);
    }

    void TextArea::setScrollBarTrackColor(const sf::Color& c)
    {
        scrollBarTrackColor = c;
        _scrollPane.setBackgroundColor(c);
    }

    void TextArea::setScrollBarWidth(float w)
    {
        scrollBarWidth = w;
        _scrollPane.setScrollBarWidth(w);
    }

    // ── Size / position ───────────────────────────────────────────────────────

    void TextArea::setSize(const sf::Vector2f& sz)
    {
        // TextInput::setSize resizes canvas to sz — onRebuildComplete will
        // immediately grow it back to content height after the next rebuild.
        TextInput::setSize(sz);
        _scrollPane.setSize(sz.x, sz.y);
        _scrollPane.setPosition(_position);
        // Update word wrap width when field size changes
        _renderer.setMaxWidth(sz.x - padding * 2.f - scrollBarWidth);
    }

    void TextArea::setPosition(const sf::Vector2f& pos)
    {
        TextInput::setPosition(pos);
        _scrollPane.setPosition(pos);
    }

    sf::FloatRect TextArea::getGlobalBounds() const
    {
        return _background.getGlobalBounds();
    }

} // namespace ml
