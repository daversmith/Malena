// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    Select::Select(const sf::Font& font_)
        : _triggerLabel(font_),
          _arrow(font_)
    {
        // Seed from active theme, then apply the constructor font
        SelectTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _trigger.setSize(size);
        _trigger.setFillColor(bgIdle);
        _trigger.setOutlineColor(borderIdle);
        _trigger.setOutlineThickness(borderThickness);

        _triggerLabel.setCharacterSize(fontSize);
        _triggerLabel.setFillColor(placeholderColor);
        _triggerLabel.setString(_placeholder);

        _arrow.setCharacterSize(fontSize);
        _arrow.setFillColor(arrowColor);
        _arrow.setString(L"\u25BC");

        _panel.setFillColor(panelBg);
        _panel.setOutlineColor(panelBorder);
        _panel.setOutlineThickness(panelBorderThick);

        resizePanelCanvas();
        syncTrigger();

        onHover([this]{
            if (checkFlag(Flag::DISABLED) || checkFlag(Flag::OPEN)) return;
            setState(State::HOVERED);
            syncTriggerColors();
        });

        onUnhover([this]{
            if (checkFlag(Flag::DISABLED) || checkFlag(Flag::OPEN)) return;
            setState(State::IDLE);
            syncTriggerColors();
        });

        onClick([this]{
            if (checkFlag(Flag::DISABLED)) return;
            if (checkFlag(Flag::OPEN)) closePanel();
            else                       openPanel();
        });

        onUpdate([this]{
            if (!checkFlag(Flag::OPEN)) { _hoveredIndex = -1; return; }

            const sf::Vector2f wp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));

            _hoveredIndex = hitTestPanel(wp);

            static bool prevDown = false;
            const bool  mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            if (mouseDown && !prevDown)
            {
                const int idx = hitTestPanel(wp);
                if (idx >= 0 && idx < static_cast<int>(_options.size()))
                {
                    if (_options[idx].enabled)
                    {
                        commitSelection(idx);
                        closePanel();
                    }
                }
                else
                {
                    const sf::FloatRect tb = _trigger.getGlobalBounds();
                    const sf::FloatRect pb = _panel.getGlobalBounds();
                    if (!tb.contains(wp) && !pb.contains(wp))
                        closePanel();
                }
            }
            prevDown = mouseDown;
        });

        onScroll([this](const std::optional<sf::Event>& e){
            if (!checkFlag(Flag::OPEN) || !e) return;
            const auto* scroll = e->getIf<sf::Event::MouseWheelScrolled>();
            if (!scroll) return;
            const float maxScroll = std::max(0.f,
                static_cast<float>(_options.size()) * itemHeight - panelHeight());
            _scrollOffset -= scroll->delta * itemHeight;
            _scrollOffset  = std::clamp(_scrollOffset, 0.f, maxScroll);
        });
    }

    void Select::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        SelectTheme::applyFrom(theme);
        _trigger.setOutlineThickness(borderThickness);
        _panel.setFillColor(panelBg);
        _panel.setOutlineColor(panelBorder);
        _panel.setOutlineThickness(panelBorderThick);
        _arrow.setFillColor(arrowColor);
        _triggerLabel.setCharacterSize(fontSize);
        _arrow.setCharacterSize(fontSize);
        syncTriggerColors();
        syncTrigger();
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void Select::syncTrigger()
    {
        _trigger.setSize(size);
        _trigger.setPosition(_position);

        const sf::FloatRect lb = _triggerLabel.getLocalBounds();
        _triggerLabel.setPosition({
            _position.x + padding,
            _position.y + (size.y - lb.size.y) / 2.f - lb.position.y
        });

        const sf::FloatRect ab = _arrow.getLocalBounds();
        _arrow.setPosition({
            _position.x + size.x - padding - ab.size.x - ab.position.x,
            _position.y + (size.y - ab.size.y) / 2.f - ab.position.y
        });
    }

    void Select::syncTriggerColors()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool open     = checkFlag(Flag::OPEN);
        const bool hovered  = isState(State::HOVERED);
        const bool hasValue = _selectedIndex >= 0;

        _trigger.setFillColor(
            disabled ? bgDisabled :
            open     ? bgOpen     :
            hovered  ? bgHovered  : bgIdle);

        _trigger.setOutlineColor(
            disabled ? borderDisabled :
            open     ? borderOpen     : borderIdle);

        _triggerLabel.setFillColor(
            disabled ? labelDisabled :
            hasValue ? labelColor    : placeholderColor);
    }

    void Select::syncPanel()
    {
        const float ph = panelHeight();
        const float py = panelY();
        _panel.setSize({size.x, ph});
        _panel.setPosition({_position.x, py});
        resizePanelCanvas();
    }

    float Select::panelHeight() const
    {
        const float rows = std::min(static_cast<float>(_options.size()), maxVisibleItems);
        return rows * itemHeight;
    }

    float Select::panelY() const
    {
        const float ph   = panelHeight();
        const float winH = static_cast<float>(
            WindowManager::getWindow().getSize().y);
        const float spaceBelow = winH - (_position.y + size.y);
        _openAbove = (spaceBelow < ph && _position.y > ph);
        return _openAbove ? _position.y - ph : _position.y + size.y;
    }

    void Select::resizePanelCanvas()
    {
        const unsigned int w = std::max(1u, static_cast<unsigned int>(size.x));
        const unsigned int h = std::max(1u, static_cast<unsigned int>(
            maxVisibleItems * itemHeight + 2.f));
        _panelCanvas.resize({w, h});
    }

    void Select::openPanel()
    {
        _scrollOffset = 0.f;
        syncPanel();
        enableFlag(Flag::OPEN);
        setState(State::OPEN);
        _arrow.setString(L"\u25B2");
        syncTriggerColors();
    }

    void Select::closePanel()
    {
        disableFlag(Flag::OPEN);
        setState(State::IDLE);
        _hoveredIndex = -1;
        _arrow.setString(L"\u25BC");
        syncTriggerColors();
    }

    void Select::commitSelection(int index)
    {
        if (index < 0 || index >= static_cast<int>(_options.size())) return;
        if (!_options[index].enabled) return;

        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_options.size()))
            _options[_selectedIndex].selected = false;

        _selectedIndex = index;
        _options[index].selected = true;

        const std::string& lbl = _options[index].label.empty()
                                 ? _options[index].value
                                 : _options[index].label;
        _triggerLabel.setString(lbl);

        if (_options[index].style.color.has_value())
            _triggerLabel.setFillColor(*_options[index].style.color);

        syncTrigger();

        if (_onSelectionChanged)
            _onSelectionChanged(_options[index].value,
                                static_cast<std::size_t>(index));
    }

    int Select::hitTestPanel(const sf::Vector2f& worldPos) const
    {
        const sf::FloatRect pb = _panel.getGlobalBounds();
        if (!pb.contains(worldPos)) return -1;

        const float relY    = worldPos.y - pb.position.y + _scrollOffset;
        const int   itemIdx = static_cast<int>(relY / itemHeight);

        if (itemIdx < 0 || itemIdx >= static_cast<int>(_options.size())) return -1;
        return itemIdx;
    }

    void Select::drawOption(sf::RenderTarget&       target,
                            const sf::RenderStates& states,
                            const SelectOption&     option,
                            float                   itemY,
                            bool                    hovered,
                            bool                    selected) const
    {
        const float x = _position.x;

        if (option.customComponent)
        {
            option.customComponent->setPosition({x, itemY});
            target.draw(*dynamic_cast<sf::Drawable*>(option.customComponent), states);
            return;
        }

        sf::RectangleShape bg({size.x, itemHeight});
        bg.setPosition({x, itemY});

        if (!option.enabled)
            bg.setFillColor(sf::Color::Transparent);
        else if (option.style.bgColor.has_value())
            bg.setFillColor(*option.style.bgColor);
        else if (selected)
            bg.setFillColor(itemSelectedBg);
        else if (hovered)
            bg.setFillColor(itemHoverBg);
        else
            bg.setFillColor(sf::Color::Transparent);

        target.draw(bg, states);

        float currentX = x + padding;

        if (option.style.iconTexture)
        {
            sf::Sprite icon(*option.style.iconTexture);
            if (option.style.iconRect != sf::IntRect{})
                icon.setTextureRect(option.style.iconRect);

            const sf::Vector2f iconSz = option.style.iconSize.x > 0
                ? option.style.iconSize
                : sf::Vector2f{
                    static_cast<float>(icon.getTextureRect().size.x),
                    static_cast<float>(icon.getTextureRect().size.y)
                };

            const auto native = icon.getTextureRect().size;
            icon.setScale({
                iconSz.x / static_cast<float>(native.x),
                iconSz.y / static_cast<float>(native.y)
            });
            icon.setPosition({currentX, itemY + (itemHeight - iconSz.y) / 2.f});
            target.draw(icon, states);
            currentX += iconSz.x + padding;
        }

        const sf::Font& f     = option.style.font.has_value()
                                ? *(*option.style.font) : *font;
        const unsigned int sz = option.style.charSize.value_or(fontSize);

        sf::Color labelCol = option.enabled
            ? option.style.color.value_or(itemLabelColor)
            : itemDisabledColor;

        sf::Text label(f, option.label, sz);
        label.setFillColor(labelCol);

        sf::Text::Style style = sf::Text::Style::Regular;
        if (option.style.bold.value_or(false))
            style = static_cast<sf::Text::Style>(
                static_cast<int>(style) | static_cast<int>(sf::Text::Style::Bold));
        if (option.style.italic.value_or(false))
            style = static_cast<sf::Text::Style>(
                static_cast<int>(style) | static_cast<int>(sf::Text::Style::Italic));
        label.setStyle(style);

        const bool hasDesc = !option.style.description.empty();

        if (hasDesc)
        {
            const float halfH = itemHeight / 2.f;
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                currentX,
                itemY + halfH / 2.f - lb.size.y / 2.f - lb.position.y
            });
        }
        else
        {
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                currentX,
                itemY + (itemHeight - lb.size.y) / 2.f - lb.position.y
            });
        }
        target.draw(label, states);

        if (hasDesc)
        {
            const unsigned int dSz  = option.style.descCharSize.value_or(descCharSize);
            const sf::Color    dCol = option.enabled
                ? option.style.descColor.value_or(itemDescColor)
                : itemDisabledColor;

            sf::Text desc(f, option.style.description, dSz);
            desc.setFillColor(dCol);

            const float halfH = itemHeight / 2.f;
            const sf::FloatRect db = desc.getLocalBounds();
            desc.setPosition({
                currentX,
                itemY + halfH + halfH / 2.f - db.size.y / 2.f - db.position.y
            });
            target.draw(desc, states);
        }
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void Select::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_trigger,      states);
        target.draw(_triggerLabel, states);
        target.draw(_arrow,        states);

        if (!checkFlag(Flag::OPEN)) return;

        target.draw(_panel, states);

        _panelCanvas.clear(sf::Color::Transparent);

        const float panelOriginY = _panel.getPosition().y;

        sf::RenderStates cs;
        cs.transform.translate({-_position.x, -panelOriginY});

        for (int i = 0; i < static_cast<int>(_options.size()); ++i)
        {
            const float itemWorldY = panelOriginY
                                   + static_cast<float>(i) * itemHeight
                                   - _scrollOffset;

            if (itemWorldY + itemHeight < panelOriginY) continue;
            if (itemWorldY > panelOriginY + panelHeight())  continue;

            drawOption(_panelCanvas, cs,
                       _options[i], itemWorldY,
                       i == _hoveredIndex,
                       _options[i].selected);
        }

        _panelCanvas.display();

        sf::Sprite sprite(_panelCanvas.getTexture());
        sprite.setPosition({_position.x, panelOriginY});
        target.draw(sprite, states);
    }

    // ── Adding options ────────────────────────────────────────────────────────

    void Select::addOption(const std::string& label, const std::string& value,
                           const SelectOptionStyle& style)
    { _options.emplace_back(label, value, style); syncPanel(); }

    void Select::add(ml::Core& component, const std::string& value)
    { _options.emplace_back(component, value); syncPanel(); }

    void Select::clearOptions()
    {
        _options.clear();
        _selectedIndex = -1;
        _triggerLabel.setString(_placeholder);
        _triggerLabel.setFillColor(placeholderColor);
        syncTrigger();
        syncPanel();
    }

    std::size_t Select::optionCount() const { return _options.size(); }

    // ── Selection ─────────────────────────────────────────────────────────────

    void Select::selectIndex(std::size_t i) { commitSelection(static_cast<int>(i)); }

    void Select::selectValue(const std::string& value)
    {
        for (std::size_t i = 0; i < _options.size(); ++i)
            if (_options[i].value == value) { commitSelection(static_cast<int>(i)); return; }
    }

    void Select::clearSelection()
    {
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_options.size()))
            _options[_selectedIndex].selected = false;
        _selectedIndex = -1;
        _triggerLabel.setString(_placeholder);
        syncTriggerColors();
        syncTrigger();
    }

    int         Select::getSelectedIndex() const { return _selectedIndex; }
    std::string Select::getSelectedLabel() const
    { return _selectedIndex < 0 ? "" : _options[_selectedIndex].label; }
    std::string Select::getSelectedValue() const
    { return _selectedIndex < 0 ? "" : _options[_selectedIndex].value; }

    // ── Open / close ──────────────────────────────────────────────────────────

    void Select::open()  { if (!checkFlag(Flag::OPEN)) openPanel(); }
    void Select::close() { if (checkFlag(Flag::OPEN))  closePanel(); }
    bool Select::isOpen() const { return checkFlag(Flag::OPEN); }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void Select::onSelectionChanged(
        std::function<void(const std::string&, std::size_t)> cb)
    { _onSelectionChanged = std::move(cb); }

    // ── Placeholder ───────────────────────────────────────────────────────────

    void Select::setPlaceholder(const std::string& text)
    {
        _placeholder = text;
        if (_selectedIndex < 0) { _triggerLabel.setString(text); syncTrigger(); }
    }

    std::string Select::getPlaceholder() const { return _placeholder; }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void Select::setEnabled(bool enabled)
    {
        if (enabled) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else         { enableFlag(Flag::DISABLED);  setState(State::DISABLED); closePanel(); }
        syncTriggerColors();
    }

    bool Select::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void Select::setOptionEnabled(std::size_t index, bool enabled)
    { if (index < _options.size()) _options[index].enabled = enabled; }

    // ── Font ──────────────────────────────────────────────────────────────────

    void Select::setFont(const sf::Font& f)
    {
        font = &f;
        _triggerLabel.setFont(f);
        _arrow.setFont(f);
        syncTrigger();
    }

    unsigned int Select::getCharacterSize() const { return fontSize; }

    // ── Positionable ──────────────────────────────────────────────────────────

    void Select::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        syncTrigger();
        if (checkFlag(Flag::OPEN)) syncPanel();
    }

    sf::Vector2f  Select::getPosition()     const { return _position; }
    sf::FloatRect Select::getGlobalBounds() const { return _trigger.getGlobalBounds(); }

} // namespace ml