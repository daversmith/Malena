// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/TextArea.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace ml
{
    TextArea::TextArea(const sf::Font& font)
        : TextInput(font),
          _scrollPane(_size.x, _size.y)
    {
        // Prevent ScrollPane from intercepting focus/click events —
        // it sits on top of TextArea visually and would steal focus.
        _scrollPane.unsubscribe(ml::Event::CLICK);
        _scrollPane.unsubscribe(ml::Event::FOCUS);

        _scrollPane.setPosition(_position);

        // Additional keypress handler for multiline-specific keys.
        // TextInput's onKeypress also fires and handles everything else.
        onKeypress([this](const std::optional<sf::Event>& e){
            if (!checkFlag(ml::Flag::FOCUSED)) return;
            if (checkFlag(Flag::DISABLED))     return;
            if (!e) return;
            if (const auto* kp = e->getIf<sf::Event::KeyPressed>())
                handleTextAreaKeypress(*kp);
        });
    }

    // ── reflow override — top-aligned instead of vertically centred ───────────

    void TextArea::reflow()
    {
        _renderer.setOrigin({_padding - _scrollX, _padding});
        syncPlaceholder();
    }

    void TextArea::syncPlaceholder()
    {
        _showPlaceholder = _buffer.empty();
        const sf::FloatRect lb = _placeholder.getLocalBounds();
        _placeholder.setPosition({
            _position.x + _padding,
            _position.y + _padding - lb.position.y
        });
    }

    // ── Keypress ──────────────────────────────────────────────────────────────

    void TextArea::handleTextAreaKeypress(const sf::Event::KeyPressed& kp)
    {
        const bool shift = kp.shift;
        const bool ctrl  = kp.control || kp.system;

        // Ctrl+Enter — submit
        if (kp.code == sf::Keyboard::Key::Enter && ctrl)
        {
            if (_onSubmit) _onSubmit(_buffer.getText());
            return;
        }

        // Enter — insert newline
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

        // Up arrow — move to line above
        if (kp.code == sf::Keyboard::Key::Up)
        {
            const std::size_t newIdx = _renderer.charIndexAbove(_buffer.getCursor());
            if (shift) _buffer.setSelection(_buffer.getCursor(), newIdx);
            else       _buffer.setCursor(newIdx);
            rebuildAndScroll();
            _cursorClock.restart();
            _cursorVisible = true;
            return;
        }

        // Down arrow — move to line below
        if (kp.code == sf::Keyboard::Key::Down)
        {
            const std::size_t newIdx = _renderer.charIndexBelow(_buffer.getCursor());
            if (shift) _buffer.setSelection(_buffer.getCursor(), newIdx);
            else       _buffer.setCursor(newIdx);
            rebuildAndScroll();
            _cursorClock.restart();
            _cursorVisible = true;
            return;
        }

        // All other keys handled by TextInput's onKeypress
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void TextArea::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_background, states);
        target.draw(_scrollPane, states);

        if (_showPlaceholder)
            target.draw(_placeholder, states);

        // Text, selection and cursor through canvas — same as TextInput
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

        if (_cursorVisible
            && checkFlag(ml::Flag::FOCUSED)
            && !checkFlag(Flag::DISABLED)
            && !_dragging)
        {
            _renderer.drawCursor(_canvas, cs, _buffer.getCursor(), _cursorColor);
        }

        _canvas.display();

        sf::Sprite sprite(_canvas.getTexture());
        sprite.setPosition(_position);
        target.draw(sprite, states);
    }

    // ── Scrollbar styling ─────────────────────────────────────────────────────

    void TextArea::setScrollBarColor(const sf::Color& color)
    {
        _scrollPane.setScrollBarColor(color);
    }

    void TextArea::setScrollBarTrackColor(const sf::Color& color)
    {
        _scrollPane.setBackgroundColor(color);
    }

    void TextArea::setScrollBarWidth(float width)
    {
        _scrollPane.setScrollBarWidth(width);
    }

    // ── Overrides ─────────────────────────────────────────────────────────────

    void TextArea::setSize(const sf::Vector2f& size)
    {
        TextInput::setSize(size);
        _scrollPane.setSize(size.x, size.y);
        _scrollPane.setPosition(_position);
    }

    void TextArea::setPosition(const sf::Vector2f& position)
    {
        TextInput::setPosition(position);
        _scrollPane.setPosition(position);
    }

    sf::FloatRect TextArea::getGlobalBounds() const
    {
        return _background.getGlobalBounds();
    }

} // namespace ml