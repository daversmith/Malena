// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/ToggleGroup.h>
#include <algorithm>

namespace ml
{
    // ── Item accessors ────────────────────────────────────────────────────────

    bool ToggleGroup::Item::isOn() const
    {
        if (pill)    return pill->isOn();
        if (segment) return segment->isOn();
        if (button)  return button->isOn();
        return false;
    }

    void ToggleGroup::Item::setOn(bool on)
    {
        if (pill)    pill->setOn(on);
        if (segment) segment->setOn(on);
        if (button)  button->setOn(on);
    }

    sf::Vector2f ToggleGroup::Item::pos() const
    {
        if (pill)    return pill->getPosition();
        if (segment) return segment->getPosition();
        if (button)  return button->getPosition();
        return {};
    }

    void ToggleGroup::Item::setPos(const sf::Vector2f& p)
    {
        if (pill)    pill->setPosition(p);
        if (segment) segment->setPosition(p);
        if (button)  button->setPosition(p);
    }

    sf::FloatRect ToggleGroup::Item::bounds() const
    {
        if (pill)    return pill->getGlobalBounds();
        if (segment) return segment->getGlobalBounds();
        if (button)  return button->getGlobalBounds();
        return {};
    }

    // ── ToggleGroup ───────────────────────────────────────────────────────────

    ToggleGroup::ToggleGroup(const sf::Font& font)
        : _font(&font)
    {}

    void ToggleGroup::relayout()
    {
        float y = _position.y;
        for (auto& item : _items)
        {
            item.setPos({_position.x, y});
            y += item.bounds().size.y + _spacing;
        }
    }

    void ToggleGroup::attach(Item& item)
    {
        const std::string key = item.key;
        auto cb = [this, key](bool on){
            if (_onAnyToggled) _onAnyToggled(key, on);
        };
        if (item.pill)    item.pill->onToggled(cb);
        if (item.segment) item.segment->onToggled(cb);
        if (item.button)  item.button->onToggled(cb);
    }

    void ToggleGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (const auto& item : _items)
        {
            if (item.pill)    target.draw(*item.pill,    states);
            if (item.segment) target.draw(*item.segment, states);
            if (item.button)  target.draw(*item.button,  states);
        }
    }

    PillToggle& ToggleGroup::addToggle(const std::string& label,
                                        const std::string& key,
                                        bool on)
    {
        // Create PillToggle on heap — owned by _ownedPills
        auto t = std::make_unique<PillToggle>();
        t->setFont(*_font);
        t->setCharacterSize(_charSize);
        t->setRightLabel(label);
        if (on) t->setOn(true);

        PillToggle* ptr = t.get();
        _ownedPills.push_back(std::move(t));

        // Build item using only the pill pointer — no sf::Drawable cast needed
        Item item;
        item.type  = ItemType::PILL;
        item.key   = key;
        item.pill  = ptr;
        item.owned = true;

        attach(item);
        _items.push_back(std::move(item));
        relayout();
        return *ptr;
    }

    void ToggleGroup::add(PillToggle& toggle, const std::string& key)
    {
        Item item;
        item.type = ItemType::PILL;
        item.key  = key;
        item.pill = &toggle;
        attach(item);
        _items.push_back(std::move(item));
        relayout();
    }

    void ToggleGroup::add(SegmentToggle& toggle, const std::string& key)
    {
        Item item;
        item.type    = ItemType::SEGMENT;
        item.key     = key;
        item.segment = &toggle;
        attach(item);
        _items.push_back(std::move(item));
        relayout();
    }

    void ToggleGroup::add(ButtonToggle& toggle, const std::string& key)
    {
        Item item;
        item.type   = ItemType::BUTTON;
        item.key    = key;
        item.button = &toggle;
        attach(item);
        _items.push_back(std::move(item));
        relayout();
    }

    void ToggleGroup::clearToggles()
    {
        _items.clear();
        _ownedPills.clear();
    }

    bool ToggleGroup::isOn(const std::string& key) const
    {
        for (const auto& item : _items)
            if (item.key == key) return item.isOn();
        return false;
    }

    void ToggleGroup::setOn(const std::string& key, bool on)
    {
        for (auto& item : _items)
            if (item.key == key) { item.setOn(on); return; }
    }

    void ToggleGroup::setAllOn()
    { for (auto& item : _items) item.setOn(true); }

    void ToggleGroup::setAllOff()
    { for (auto& item : _items) item.setOn(false); }

    std::vector<std::string> ToggleGroup::getOnKeys() const
    {
        std::vector<std::string> keys;
        for (const auto& item : _items)
            if (item.isOn()) keys.push_back(item.key);
        return keys;
    }

    void ToggleGroup::onAnyToggled(
        std::function<void(const std::string&, bool)> cb)
    { _onAnyToggled = std::move(cb); }

    void ToggleGroup::setSpacing(float s) { _spacing = s; relayout(); }

    void ToggleGroup::setFont(const sf::Font& f)
    { _font = &f; relayout(); }

    void ToggleGroup::setCharacterSize(unsigned int s)
    { _charSize = s; relayout(); }

    void ToggleGroup::setPosition(const sf::Vector2f& pos)
    { _position = pos; relayout(); }

    sf::Vector2f ToggleGroup::getPosition() const { return _position; }

    sf::FloatRect ToggleGroup::getGlobalBounds() const
    {
        if (_items.empty()) return {};

        sf::FloatRect b   = _items.front().bounds();
        float right  = b.position.x + b.size.x;
        float bottom = b.position.y + b.size.y;

        for (const auto& item : _items)
        {
            const sf::FloatRect ib = item.bounds();
            right  = std::max(right,  ib.position.x + ib.size.x);
            bottom = std::max(bottom, ib.position.y + ib.size.y);
        }

        b.size.x = right  - b.position.x;
        b.size.y = bottom - b.position.y;
        return b;
    }

} // namespace ml