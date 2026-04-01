// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Utilities/Align.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    ListItem::ListItem(const sf::Font& font_)
        : _label(font_),
          _description(font_)
    {
        ListItemTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _label.setCharacterSize(fontSize);
        _label.setFillColor(textColor);

        _description.setCharacterSize(fontSizeSmall);
        _description.setFillColor(mutedColor);

        _background.setFillColor(bgIdle);

        // Hover
        onHover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::HOVERED);
            applyVisualState();
        });
        onUnhover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::IDLE);
            applyVisualState();
        });

        // Click — fire user callback
        this->onClick([this]{
            if (!checkFlag(Flag::DISABLED) && _onClickCb)
                _onClickCb();
        });

        setState(State::IDLE);
        layout();
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void ListItem::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        ListItemTheme::applyFrom(theme);
        this->font = font; // stays as-is; ThemeManager sets theme.font
        _label.setCharacterSize(fontSize);
        _label.setFillColor(textColor);
        _description.setCharacterSize(fontSizeSmall);
        _description.setFillColor(mutedColor);
        applyVisualState();
        layout();
    }

    // ── applyVisualState ──────────────────────────────────────────────────────

    void ListItem::applyVisualState()
    {
        if (checkFlag(Flag::DISABLED))
        {
            _background.setFillColor(bgDisabled);
            _label.setFillColor(disabledTextColor);
            _description.setFillColor(disabledTextColor);
        }
        else if (isState(State::HOVERED))
        {
            _background.setFillColor(bgHover);
            _label.setFillColor(textColor);
            _description.setFillColor(mutedColor);
        }
        else
        {
            _background.setFillColor(bgIdle);
            _label.setFillColor(textColor);
            _description.setFillColor(mutedColor);
        }
    }

    // ── layout ────────────────────────────────────────────────────────────────

    float ListItem::contentHeight() const
    {
        if (rowHeight > 0.f) return rowHeight;

        float h = static_cast<float>(fontSize);

        if (_hasCustomContent && _content)
            h = _content->getGlobalBounds().size.y;
        else if (_hasDescription)
            h = static_cast<float>(fontSize) + static_cast<float>(fontSizeSmall) + 4.f;

        // Also consider slot heights
        if (_start) h = std::max(h, _start->getGlobalBounds().size.y);
        if (_end)   h = std::max(h, _end->getGlobalBounds().size.y);

        return h + padding * 2.f;
    }

    void ListItem::layout()
    {
        const float h      = contentHeight();
        const float halfH  = h / 2.f;
        const float cy     = _position.y + halfH;  // vertical center

        _background.setSize({_width, h});
        _background.setPosition(_position);

        float leftX  = _position.x + padding;
        float rightX = _position.x + _width - padding;

        // ── End slot — right-aligned first so content knows how much space is left
        if (_end)
        {
            const float endW = _end->getGlobalBounds().size.x;
            const float endH = _end->getGlobalBounds().size.y;
            rightX -= endW;
            _end->setPosition({rightX, cy - endH / 2.f});
            rightX -= slotGap;
        }

        // ── Start slot
        if (_start)
        {
            const float startH = _start->getGlobalBounds().size.y;
            _start->setPosition({leftX, cy - startH / 2.f});
            leftX += _start->getGlobalBounds().size.x + slotGap;
        }

        // ── Content area
        const float contentW = std::max(0.f, rightX - leftX);

        if (_hasCustomContent && _content)
        {
            const float cH = _content->getGlobalBounds().size.y;
            _content->setPosition({leftX, cy - cH / 2.f});
        }
        else
        {
            // Built-in label (+ optional description)
            if (_hasDescription)
            {
                const float totalTextH = static_cast<float>(fontSize)
                                       + static_cast<float>(fontSizeSmall) + 4.f;
                const float textStartY = cy - totalTextH / 2.f;

                _label.setPosition({leftX, textStartY});
                _description.setPosition({leftX, textStartY + static_cast<float>(fontSize) + 4.f});
            }
            else
            {
                // Vertically center single label
                const sf::FloatRect lb = _label.getLocalBounds();
                _label.setPosition({leftX, cy - lb.size.y / 2.f - lb.position.y});
            }
        }
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void ListItem::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_background, states);

        if (_start)
            target.draw(*dynamic_cast<sf::Drawable*>(_start), states);

        if (_hasCustomContent && _content)
            target.draw(*dynamic_cast<sf::Drawable*>(_content), states);
        else
        {
            target.draw(_label, states);
            if (_hasDescription)
                target.draw(_description, states);
        }

        if (_end)
            target.draw(*dynamic_cast<sf::Drawable*>(_end), states);
    }

    // ── Slots ─────────────────────────────────────────────────────────────────

    void ListItem::setStart(ml::Core& c)
    {
        _start = &c;
        layout();
    }

    void ListItem::setEnd(ml::Core& c)
    {
        _end = &c;
        layout();
    }

    void ListItem::setContent(ml::Core& c)
    {
        _content           = &c;
        _hasCustomContent  = true;
        layout();
    }

    // ── Built-in content ──────────────────────────────────────────────────────

    void ListItem::setLabel(const std::string& text)
    {
        _label.setString(text);
        layout();
    }

    std::string ListItem::getLabel() const
    {
        return _label.getString();
    }

    void ListItem::setDescription(const std::string& text)
    {
        _description.setString(text);
        _hasDescription = !text.empty();
        layout();
    }

    std::string ListItem::getDescription() const
    {
        return _description.getString();
    }

    // ── Click ─────────────────────────────────────────────────────────────────

    void ListItem::onClick(std::function<void()> cb)
    {
        _onClickCb = std::move(cb);
    }

    // ── State ─────────────────────────────────────────────────────────────────

    void ListItem::setEnabled(bool enabled)
    {
        if (enabled) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else         { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        applyVisualState();
    }

    bool ListItem::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    // ── Width ─────────────────────────────────────────────────────────────────

    void ListItem::setWidth(float w)
    {
        _width = w;
        layout();
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void ListItem::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
    }

    sf::Vector2f  ListItem::getPosition()     const { return _position; }
    sf::FloatRect ListItem::getGlobalBounds() const { return _background.getGlobalBounds(); }

} // namespace ml
