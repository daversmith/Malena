// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/TextInputBase.h>

namespace ml
{
    TextInputBase::TextInputBase(const sf::Font& font)
        : _font(&font),
          _placeholder(font)
    {
        _background.setSize(_size);
        _background.setFillColor(_bgColor);
        _background.setOutlineColor(_borderColor);
        _background.setOutlineThickness(_borderThickness);

        _placeholder.setCharacterSize(_charSize);
        _placeholder.setFillColor(_placeholderColor);

        setState(State::IDLE);

        // Focus / blur wiring
        onFocus([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::FOCUSED);
            applyBackgroundState();
        });

        onBlur([this]{
            if (isState(State::ERROR)) return;
            setState(State::IDLE);
            applyBackgroundState();
        });
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void TextInputBase::applyBackgroundState()
    {
        if (checkFlag(Flag::DISABLED))
        {
            _background.setFillColor(_bgDisabledColor);
            _background.setOutlineColor(_borderDisabledColor);
        }
        else if (isState(State::ERROR))
        {
            _background.setFillColor(_bgColor);
            _background.setOutlineColor(_borderErrorColor);
        }
        else if (isState(State::FOCUSED))
        {
            _background.setFillColor(_bgFocusedColor);
            _background.setOutlineColor(_borderFocusedColor);
        }
        else
        {
            _background.setFillColor(_bgColor);
            _background.setOutlineColor(_borderColor);
        }
    }

    void TextInputBase::updatePlaceholderPosition()
    {
        _placeholder.setPosition({
            _position.x + _padding,
            _position.y + (_size.y - _placeholder.getLocalBounds().size.y) / 2.f
                        - _placeholder.getLocalBounds().position.y
        });
    }

    // ── Size ──────────────────────────────────────────────────────────────────

    void TextInputBase::setSize(const sf::Vector2f& size)
    {
        _size = size;
        _background.setSize(size);
        updatePlaceholderPosition();
    }

    sf::Vector2f TextInputBase::getSize() const
    {
        return _size;
    }

    // ── Placeholder ───────────────────────────────────────────────────────────

    void TextInputBase::setPlaceholder(const std::string& text)
    {
        _placeholder.setString(text);
        updatePlaceholderPosition();
    }

    std::string TextInputBase::getPlaceholder() const
    {
        return _placeholder.getString();
    }

    // ── Enabled / disabled / readonly ─────────────────────────────────────────

    void TextInputBase::setEnabled(bool enabled)
    {
        if (enabled)
        {
            disableFlag(Flag::DISABLED);
            setState(State::IDLE);
        }
        else
        {
            enableFlag(Flag::DISABLED);
            setState(State::DISABLED);
        }
        applyBackgroundState();
    }

    bool TextInputBase::isEnabled() const
    {
        return !checkFlag(Flag::DISABLED);
    }

    void TextInputBase::setReadOnly(bool readonly)
    {
        if (readonly)
            enableFlag(Flag::READONLY);
        else
            disableFlag(Flag::READONLY);
    }

    bool TextInputBase::isReadOnly() const
    {
        return checkFlag(Flag::READONLY);
    }

    // ── Error state ───────────────────────────────────────────────────────────

    void TextInputBase::setError(bool error)
    {
        if (error)
            setState(State::ERROR);
        else
            setState(isState(State::FOCUSED) ? State::FOCUSED : State::IDLE);
        applyBackgroundState();
    }

    bool TextInputBase::hasError() const
    {
        return isState(State::ERROR);
    }

    // ── Styling ───────────────────────────────────────────────────────────────

    void TextInputBase::setBackgroundColor(const sf::Color& color)
    {
        _bgColor = color;
        applyBackgroundState();
    }

    void TextInputBase::setBackgroundFocusedColor(const sf::Color& color)
    {
        _bgFocusedColor = color;
        applyBackgroundState();
    }

    void TextInputBase::setBorderColor(const sf::Color& color)
    {
        _borderColor = color;
        applyBackgroundState();
    }

    void TextInputBase::setBorderFocusedColor(const sf::Color& color)
    {
        _borderFocusedColor = color;
        applyBackgroundState();
    }

    void TextInputBase::setBorderErrorColor(const sf::Color& color)
    {
        _borderErrorColor = color;
        applyBackgroundState();
    }

    void TextInputBase::setBorderThickness(float thickness)
    {
        _borderThickness = thickness;
        _background.setOutlineThickness(thickness);
    }

    void TextInputBase::setTextColor(const sf::Color& color)
    {
        _textColor = color;
    }

    void TextInputBase::setPlaceholderColor(const sf::Color& color)
    {
        _placeholderColor = color;
        _placeholder.setFillColor(color);
    }

    void TextInputBase::setPadding(float padding)
    {
        _padding = padding;
        updatePlaceholderPosition();
    }

    float TextInputBase::getPadding() const
    {
        return _padding;
    }

    // ── Font ──────────────────────────────────────────────────────────────────

    void TextInputBase::setFont(const sf::Font& font)
    {
        _font = &font;
        _placeholder.setFont(font);
        updatePlaceholderPosition();
    }

    void TextInputBase::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        _placeholder.setCharacterSize(size);
        updatePlaceholderPosition();
    }

    unsigned int TextInputBase::getCharacterSize() const
    {
        return _charSize;
    }

    // ── Callback ─────────────────────────────────────────────────────────────

    void TextInputBase::onChange(std::function<void(const std::string&)> callback)
    {
        _onChange = std::move(callback);
    }

    // ── Positionable overrides ────────────────────────────────────────────────

    void TextInputBase::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        _background.setPosition(position);
        updatePlaceholderPosition();
    }

    sf::Vector2f TextInputBase::getPosition() const
    {
        return _position;
    }

    sf::FloatRect TextInputBase::getGlobalBounds() const
    {
        return _background.getGlobalBounds();
    }

} // namespace ml
