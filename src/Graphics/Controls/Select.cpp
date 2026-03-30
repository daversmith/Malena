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
    Select::Select(const sf::Font& font)
        : _font(&font),
          _triggerLabel(font),
          _arrow(font)
    {
        _trigger.setSize(_size);
        _trigger.setFillColor(_trigBgIdle);
        _trigger.setOutlineColor(_trigBorder);
        _trigger.setOutlineThickness(_trigBorderThick);

        _triggerLabel.setCharacterSize(_charSize);
        _triggerLabel.setFillColor(_phColor);
        _triggerLabel.setString(_placeholder);

        _arrow.setCharacterSize(_charSize);
        _arrow.setFillColor(_arrowColor);
        _arrow.setString(L"\u25BC");

        _panel.setFillColor(_panelBg);
        _panel.setOutlineColor(_panelBorder);
        _panel.setOutlineThickness(_panelBorderThick);

        resizePanelCanvas();
        syncTrigger();

        // ── Hover ─────────────────────────────────────────────────────────────
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

        // ── Click trigger — toggle panel ──────────────────────────────────────
        onClick([this]{
            if (checkFlag(Flag::DISABLED)) return;
            if (checkFlag(Flag::OPEN)) closePanel();
            else                       openPanel();
        });

        // ── Update — panel interaction + close on outside click ───────────────
        onUpdate([this]{
            if (!checkFlag(Flag::OPEN)) { _hoveredIndex = -1; return; }

            const sf::Vector2f wp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));

            // Update hover
            _hoveredIndex = hitTestPanel(wp);

            // Handle click on panel item (poll — avoids event ordering issues)
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
                    // Click outside both trigger and panel — close
                    const sf::FloatRect tb = _trigger.getGlobalBounds();
                    const sf::FloatRect pb = _panel.getGlobalBounds();
                    if (!tb.contains(wp) && !pb.contains(wp))
                        closePanel();
                }
            }
            prevDown = mouseDown;
        });

        // ── Scroll wheel ──────────────────────────────────────────────────────
        onScroll([this](const std::optional<sf::Event>& e){
            if (!checkFlag(Flag::OPEN) || !e) return;
            const auto* scroll = e->getIf<sf::Event::MouseWheelScrolled>();
            if (!scroll) return;
            const float maxScroll = std::max(0.f,
                static_cast<float>(_options.size()) * _itemHeight - panelHeight());
            _scrollOffset -= scroll->delta * _itemHeight;
            _scrollOffset  = std::clamp(_scrollOffset, 0.f, maxScroll);
        });
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void Select::syncTrigger()
    {
        _trigger.setSize(_size);
        _trigger.setPosition(_position);

        // Trigger label — vertically centred, left-padded
        const sf::FloatRect lb = _triggerLabel.getLocalBounds();
        _triggerLabel.setPosition({
            _position.x + _padding,
            _position.y + (_size.y - lb.size.y) / 2.f - lb.position.y
        });

        // Arrow — right-padded
        const sf::FloatRect ab = _arrow.getLocalBounds();
        _arrow.setPosition({
            _position.x + _size.x - _padding - ab.size.x - ab.position.x,
            _position.y + (_size.y - ab.size.y) / 2.f - ab.position.y
        });
    }

    void Select::syncTriggerColors()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool open     = checkFlag(Flag::OPEN);
        const bool hovered  = isState(State::HOVERED);
        const bool hasValue = _selectedIndex >= 0;

        _trigger.setFillColor(
            disabled ? _trigBgDisabled :
            open     ? _trigBgOpen     :
            hovered  ? _trigBgHover    : _trigBgIdle);

        _trigger.setOutlineColor(
            disabled ? _trigBorderDis  :
            open     ? _trigBorderOpen : _trigBorder);

        _triggerLabel.setFillColor(
            disabled ? _labelDisabled :
            hasValue ? _labelColor    : _phColor);
    }

    void Select::syncPanel()
    {
        const float ph = panelHeight();
        const float py = panelY();
        _panel.setSize({_size.x, ph});
        _panel.setPosition({_position.x, py});
        resizePanelCanvas();
    }

    float Select::panelHeight() const
    {
        const float rows = std::min(static_cast<float>(_options.size()), _maxVisible);
        return rows * _itemHeight;
    }

    float Select::panelY() const
    {
        const float ph   = panelHeight();
        const float winH = static_cast<float>(
            WindowManager::getWindow().getSize().y);
        const float spaceBelow = winH - (_position.y + _size.y);
        _openAbove = (spaceBelow < ph && _position.y > ph);
        return _openAbove ? _position.y - ph : _position.y + _size.y;
    }

    void Select::resizePanelCanvas()
    {
        const unsigned int w = std::max(1u, static_cast<unsigned int>(_size.x));
        const unsigned int h = std::max(1u, static_cast<unsigned int>(
            _maxVisible * _itemHeight + 2.f));
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

        // Update trigger label — use option label or "(custom)" for components
        const std::string& lbl = _options[index].label.empty()
                                 ? _options[index].value
                                 : _options[index].label;
        _triggerLabel.setString(lbl);

        // Apply item color to trigger label if set
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
        const int   itemIdx = static_cast<int>(relY / _itemHeight);

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

        // ── Custom component ──────────────────────────────────────────────────
        if (option.customComponent)
        {
            option.customComponent->setPosition({x, itemY});
            target.draw(*dynamic_cast<sf::Drawable*>(option.customComponent), states);
            return;
        }

        // ── Background ────────────────────────────────────────────────────────
        sf::RectangleShape bg({_size.x, _itemHeight});
        bg.setPosition({x, itemY});

        if (!option.enabled)
            bg.setFillColor(sf::Color::Transparent);
        else if (option.style.bgColor.has_value())
            bg.setFillColor(*option.style.bgColor);
        else if (selected)
            bg.setFillColor(_itemSelectedBg);
        else if (hovered)
            bg.setFillColor(_itemHoverBg);
        else
            bg.setFillColor(sf::Color::Transparent);

        target.draw(bg, states);

        float currentX = x + _padding;

        // ── Icon ──────────────────────────────────────────────────────────────
        if (option.style.iconTexture)
        {
            sf::Sprite icon(*option.style.iconTexture);

            if (option.style.iconRect != sf::IntRect{})
                icon.setTextureRect(option.style.iconRect);

            const sf::Vector2f iconSize = option.style.iconSize.x > 0
                ? option.style.iconSize
                : sf::Vector2f{
                    static_cast<float>(icon.getTextureRect().size.x),
                    static_cast<float>(icon.getTextureRect().size.y)
                };

            // Scale to desired size
            const auto native = icon.getTextureRect().size;
            icon.setScale({
                iconSize.x / static_cast<float>(native.x),
                iconSize.y / static_cast<float>(native.y)
            });

            icon.setPosition({
                currentX,
                itemY + (_itemHeight - iconSize.y) / 2.f
            });
            target.draw(icon, states);
            currentX += iconSize.x + _padding;
        }

        // ── Label ─────────────────────────────────────────────────────────────
        const sf::Font& f    = option.style.font.has_value()
                               ? *(*option.style.font) : *_font;
        const unsigned int sz = option.style.charSize.value_or(_charSize);

        sf::Color labelCol = option.enabled
            ? option.style.color.value_or(_itemDefaultLabel)
            : _itemDisabledColor;

        sf::Text label(f, option.label, sz);
        label.setFillColor(labelCol);

        // Apply bold / italic
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
            // Two-line layout — label sits in top half, description in bottom
            const float halfH  = _itemHeight / 2.f;
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                currentX,
                itemY + halfH / 2.f - lb.size.y / 2.f - lb.position.y
            });
        }
        else
        {
            // Single line — vertically centred
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                currentX,
                itemY + (_itemHeight - lb.size.y) / 2.f - lb.position.y
            });
        }
        target.draw(label, states);

        // ── Description ───────────────────────────────────────────────────────
        if (hasDesc)
        {
            const unsigned int dSz = option.style.descCharSize.value_or(_descSize);
            const sf::Color    dCol = option.enabled
                ? option.style.descColor.value_or(_itemDescColor)
                : _itemDisabledColor;

            sf::Text desc(f, option.style.description, dSz);
            desc.setFillColor(dCol);

            const float halfH  = _itemHeight / 2.f;
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
        // ── Trigger ───────────────────────────────────────────────────────────
        target.draw(_trigger,      states);
        target.draw(_triggerLabel, states);
        target.draw(_arrow,        states);

        if (!checkFlag(Flag::OPEN)) return;

        // ── Panel background to main target ───────────────────────────────────
        target.draw(_panel, states);

        // ── Items drawn into canvas for clipping ──────────────────────────────
        _panelCanvas.clear(sf::Color::Transparent);

        const float panelOriginY = _panel.getPosition().y;

        // Transform: shift from world space to canvas-local space
        sf::RenderStates cs;
        cs.transform.translate({-_position.x, -panelOriginY});

        for (int i = 0; i < static_cast<int>(_options.size()); ++i)
        {
            const float itemWorldY = panelOriginY
                                   + static_cast<float>(i) * _itemHeight
                                   - _scrollOffset;

            // Cull items outside visible area
            if (itemWorldY + _itemHeight < panelOriginY) continue;
            if (itemWorldY > panelOriginY + panelHeight())  continue;

            drawOption(_panelCanvas, cs,
                       _options[i],
                       itemWorldY,
                       i == _hoveredIndex,
                       _options[i].selected);
        }

        _panelCanvas.display();

        sf::Sprite sprite(_panelCanvas.getTexture());
        sprite.setPosition({_position.x, panelOriginY});
        target.draw(sprite, states);
    }

    // ── Adding options ────────────────────────────────────────────────────────

    void Select::addOption(const std::string&       label,
                           const std::string&       value,
                           const SelectOptionStyle& style)
    {
        _options.emplace_back(label, value, style);
        syncPanel();
    }

    void Select::add(ml::Core& component, const std::string& value)
    {
        SelectOption opt(component, value);
        _options.push_back(std::move(opt));
        syncPanel();
    }

    void Select::clearOptions()
    {
        _options.clear();
        _selectedIndex = -1;
        _triggerLabel.setString(_placeholder);
        _triggerLabel.setFillColor(_phColor);
        syncTrigger();
        syncPanel();
    }

    std::size_t Select::optionCount() const { return _options.size(); }

    // ── Selection ─────────────────────────────────────────────────────────────

    void Select::selectIndex(std::size_t index)
    {
        commitSelection(static_cast<int>(index));
    }

    void Select::selectValue(const std::string& value)
    {
        for (std::size_t i = 0; i < _options.size(); ++i)
            if (_options[i].value == value)
            {
                commitSelection(static_cast<int>(i));
                return;
            }
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

    // ── Callback ─────────────────────────────────────────────────────────────

    void Select::onSelectionChanged(
        std::function<void(const std::string&, std::size_t)> cb)
    {
        _onSelectionChanged = std::move(cb);
    }

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
    {
        if (index < _options.size()) _options[index].enabled = enabled;
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void Select::setSize(const sf::Vector2f& size)
    {
        _size = size;
        syncTrigger();
        syncPanel();
        resizePanelCanvas();
    }

    sf::Vector2f Select::getSize() const { return _size; }

    void Select::setItemHeight(float h)
    { _itemHeight = h; syncPanel(); resizePanelCanvas(); }

    void Select::setMaxVisibleItems(float count)
    { _maxVisible = count; syncPanel(); resizePanelCanvas(); }

    void Select::setPadding(float p) { _padding = p; syncTrigger(); }

    // ── Styling — trigger ─────────────────────────────────────────────────────

    void Select::setBackgroundColor(const sf::Color& c)      { _trigBgIdle     = c; syncTriggerColors(); }
    void Select::setBackgroundHoverColor(const sf::Color& c) { _trigBgHover    = c; syncTriggerColors(); }
    void Select::setBackgroundOpenColor(const sf::Color& c)  { _trigBgOpen     = c; syncTriggerColors(); }
    void Select::setBorderColor(const sf::Color& c)          { _trigBorder     = c; syncTriggerColors(); }
    void Select::setBorderOpenColor(const sf::Color& c)      { _trigBorderOpen = c; syncTriggerColors(); }
    void Select::setBorderThickness(float t)                 { _trigBorderThick = t; _trigger.setOutlineThickness(t); }
    void Select::setLabelColor(const sf::Color& c)           { _labelColor     = c; syncTriggerColors(); }
    void Select::setPlaceholderColor(const sf::Color& c)     { _phColor        = c; syncTriggerColors(); }
    void Select::setArrowColor(const sf::Color& c)           { _arrowColor     = c; _arrow.setFillColor(c); }

    // ── Styling — panel ───────────────────────────────────────────────────────

    void Select::setPanelBackgroundColor(const sf::Color& c) { _panelBg          = c; _panel.setFillColor(c); }
    void Select::setPanelBorderColor(const sf::Color& c)     { _panelBorder      = c; _panel.setOutlineColor(c); }
    void Select::setItemHoverColor(const sf::Color& c)       { _itemHoverBg      = c; }
    void Select::setItemSelectedColor(const sf::Color& c)    { _itemSelectedBg   = c; }
    void Select::setItemLabelColor(const sf::Color& c)       { _itemDefaultLabel = c; }
    void Select::setItemDescriptionColor(const sf::Color& c) { _itemDescColor    = c; }

    // ── Font ──────────────────────────────────────────────────────────────────

    void Select::setFont(const sf::Font& font)
    {
        _font = &font;
        _triggerLabel.setFont(font);
        _arrow.setFont(font);
        syncTrigger();
    }

    void Select::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        _triggerLabel.setCharacterSize(size);
        _arrow.setCharacterSize(size);
        syncTrigger();
    }

    void Select::setDescriptionCharacterSize(unsigned int size) { _descSize = size; }
    unsigned int Select::getCharacterSize() const { return _charSize; }

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
