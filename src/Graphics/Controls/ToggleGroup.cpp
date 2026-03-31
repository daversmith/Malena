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

    ToggleGroup::ToggleGroup()
    {
        // Seed defaults from the active theme
        _defaultPillTheme.applyFrom(ThemeManager::get());
        _defaultSegmentTheme.applyFrom(ThemeManager::get());
        _defaultButtonTheme.applyFrom(ThemeManager::get());
    }

    ToggleGroup::~ToggleGroup()
    {
        _items.clear();
        _ownedPills.clear();
        _ownedSegments.clear();
        _ownedButtons.clear();
    }

    void ToggleGroup::relayout()
    {
        float y = _position.y;
        for (auto& p : _items)
        {
            p->setPos({_position.x, y});
            y += p->bounds().size.y + _spacing;
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
        for (const auto& p : _items)
        {
            if (p->pill)    target.draw(*p->pill,    states);
            if (p->segment) target.draw(*p->segment, states);
            if (p->button)  target.draw(*p->button,  states);
        }
    }

    // ── Adding owned toggles ──────────────────────────────────────────────────

    PillToggle& ToggleGroup::addToggle(const std::string& label,
                                        const std::string& key, bool on)
    {
        auto t = std::make_unique<PillToggle>();
        t->applySettings(_defaultPillSettings);
        t->applyTheme(_defaultPillTheme);
        t->setRightLabel(label);
        if (on) t->setOn(true);

        PillToggle* ptr = t.get();
        _ownedPills.push_back(std::move(t));

        auto item = std::make_unique<Item>();
        item->type  = ItemType::PILL;
        item->key   = key;
        item->pill  = ptr;
        item->owned = true;
        attach(*item);
        _items.push_back(std::move(item));
        relayout();
        return *ptr;
    }

    SegmentToggle& ToggleGroup::addSegmentToggle(const std::string& offLabel,
                                                  const std::string& onLabel,
                                                  const std::string& key, bool on)
    {
        auto t = std::make_unique<SegmentToggle>();
        t->applySettings(_defaultSegmentSettings);
        t->applyTheme(_defaultSegmentTheme);
        t->setSegmentLabels(offLabel, onLabel);
        if (on) t->setOn(true);

        SegmentToggle* ptr = t.get();
        _ownedSegments.push_back(std::move(t));

        auto item = std::make_unique<Item>();
        item->type    = ItemType::SEGMENT;
        item->key     = key;
        item->segment = ptr;
        item->owned   = true;
        attach(*item);
        _items.push_back(std::move(item));
        relayout();
        return *ptr;
    }

    ButtonToggle& ToggleGroup::addButtonToggle(const std::string& label,
                                                const std::string& key, bool on)
    {
        auto t = std::make_unique<ButtonToggle>();
        t->applySettings(_defaultButtonSettings);
        t->applyTheme(_defaultButtonTheme);
        t->setOnLabel(label);
        t->setOffLabel(label);
        if (on) t->setOn(true);

        ButtonToggle* ptr = t.get();
        _ownedButtons.push_back(std::move(t));

        auto item = std::make_unique<Item>();
        item->type   = ItemType::BUTTON;
        item->key    = key;
        item->button = ptr;
        item->owned  = true;
        attach(*item);
        _items.push_back(std::move(item));
        relayout();
        return *ptr;
    }

    // ── Adding external toggles ───────────────────────────────────────────────

    void ToggleGroup::add(PillToggle& toggle, const std::string& key)
    {
        auto item = std::make_unique<Item>();
        item->type = ItemType::PILL; item->key = key; item->pill = &toggle;
        attach(*item); _items.push_back(std::move(item)); relayout();
    }
    void ToggleGroup::add(SegmentToggle& toggle, const std::string& key)
    {
        auto item = std::make_unique<Item>();
        item->type = ItemType::SEGMENT; item->key = key; item->segment = &toggle;
        attach(*item); _items.push_back(std::move(item)); relayout();
    }
    void ToggleGroup::add(ButtonToggle& toggle, const std::string& key)
    {
        auto item = std::make_unique<Item>();
        item->type = ItemType::BUTTON; item->key = key; item->button = &toggle;
        attach(*item); _items.push_back(std::move(item)); relayout();
    }

    void ToggleGroup::clearToggles()
    {
        _items.clear();
        _ownedPills.clear();
        _ownedSegments.clear();
        _ownedButtons.clear();
    }

    // ── State ─────────────────────────────────────────────────────────────────

    bool ToggleGroup::isOn(const std::string& key) const
    {
        for (const auto& p : _items) if (p->key == key) return p->isOn();
        return false;
    }
    void ToggleGroup::setOn(const std::string& key, bool on)
    {
        for (auto& p : _items) if (p->key == key) { p->setOn(on); return; }
    }
    void ToggleGroup::setAllOn()  { for (auto& p : _items) p->setOn(true);  }
    void ToggleGroup::setAllOff() { for (auto& p : _items) p->setOn(false); }

    std::vector<std::string> ToggleGroup::getOnKeys() const
    {
        std::vector<std::string> keys;
        for (const auto& p : _items) if (p->isOn()) keys.push_back(p->key);
        return keys;
    }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void ToggleGroup::onAnyToggled(std::function<void(const std::string&, bool)> cb)
    { _onAnyToggled = std::move(cb); }

    // ── Default styles ────────────────────────────────────────────────────────

    void ToggleGroup::setDefaultSettings(const PillSettings& s)    { _defaultPillSettings    = s; }
    void ToggleGroup::setDefaultSettings(const SegmentSettings& s) { _defaultSegmentSettings = s; }
    void ToggleGroup::setDefaultSettings(const ButtonSettings& s)  { _defaultButtonSettings  = s; }

    void ToggleGroup::setDefaultTheme(const PillTheme& t)    { _defaultPillTheme    = t; }
    void ToggleGroup::setDefaultTheme(const SegmentTheme& t) { _defaultSegmentTheme = t; }
    void ToggleGroup::setDefaultTheme(const ButtonTheme& t)  { _defaultButtonTheme  = t; }

    void ToggleGroup::setDefaultStyle(const PillStyle& s)
    { _defaultPillSettings = s; _defaultPillTheme = s; }

    void ToggleGroup::setDefaultStyle(const SegmentStyle& s)
    { _defaultSegmentSettings = s; _defaultSegmentTheme = s; }

    void ToggleGroup::setDefaultStyle(const ButtonStyle& s)
    { _defaultButtonSettings = s; _defaultButtonTheme = s; }

    // ── Bulk apply ────────────────────────────────────────────────────────────

    void ToggleGroup::applySettingsToAll(const PillSettings& s)
    { for (auto& p : _ownedPills) p->applySettings(s); relayout(); }

    void ToggleGroup::applySettingsToAll(const SegmentSettings& s)
    { for (auto& p : _ownedSegments) p->applySettings(s); relayout(); }

    void ToggleGroup::applySettingsToAll(const ButtonSettings& s)
    { for (auto& p : _ownedButtons) p->applySettings(s); relayout(); }

    void ToggleGroup::applyThemeToAll(const PillTheme& t)
    { for (auto& p : _ownedPills) p->applyTheme(t); }

    void ToggleGroup::applyThemeToAll(const SegmentTheme& t)
    { for (auto& p : _ownedSegments) p->applyTheme(t); }

    void ToggleGroup::applyThemeToAll(const ButtonTheme& t)
    { for (auto& p : _ownedButtons) p->applyTheme(t); }

    void ToggleGroup::applyStyleToAll(const PillStyle& s)
    { for (auto& p : _ownedPills) p->applyStyle(s); relayout(); }

    void ToggleGroup::applyStyleToAll(const SegmentStyle& s)
    { for (auto& p : _ownedSegments) p->applyStyle(s); relayout(); }

    void ToggleGroup::applyStyleToAll(const ButtonStyle& s)
    { for (auto& p : _ownedButtons) p->applyStyle(s); relayout(); }

    // ── Convenience ───────────────────────────────────────────────────────────

    void ToggleGroup::setFont(const sf::Font& f)
    {
        _defaultPillTheme.setFont(f);
        for (auto& p : _ownedPills) p->setFont(f);
    }

    void ToggleGroup::setCharacterSize(unsigned int s)
    {
        _defaultPillTheme.setFontSize(s);
        for (auto& p : _ownedPills) p->setCharacterSize(s);
        relayout();
    }

    void ToggleGroup::setSpacing(float s) { _spacing = s; relayout(); }

    // ── Positionable ──────────────────────────────────────────────────────────

    void ToggleGroup::setPosition(const sf::Vector2f& pos) { _position = pos; relayout(); }
    sf::Vector2f ToggleGroup::getPosition() const          { return _position; }

    sf::FloatRect ToggleGroup::getGlobalBounds() const
    {
        if (_items.empty()) return {};
        sf::FloatRect b   = _items.front()->bounds();
        float right  = b.position.x + b.size.x;
        float bottom = b.position.y + b.size.y;
        for (const auto& p : _items)
        {
            const sf::FloatRect ib = p->bounds();
            right  = std::max(right,  ib.position.x + ib.size.x);
            bottom = std::max(bottom, ib.position.y + ib.size.y);
        }
        b.size.x = right  - b.position.x;
        b.size.y = bottom - b.position.y;
        return b;
    }

} // namespace ml