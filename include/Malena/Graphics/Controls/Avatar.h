// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Avatar — a compact identity chip: a square colour tile with the person's
// INITIALS centred on it, the tile colour derived from a hash of the name (so a
// given name is always the same colour) or an explicit seed, plus an optional
// presence dot in the corner. Reusable anywhere a roster/list shows people.
//
#ifndef MALENA_AVATAR_H
#define MALENA_AVATAR_H

#include <Malena/Core/Component.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

namespace ml {

class Avatar : public Component<>
{
    Rectangle   _chip;
    Text        _label;
    Rectangle   _dot;                 // presence indicator (hidden unless set)
    sf::Vector2f _pos;
    float       _size    = 26.f;
    bool        _showDot = false;
    bool        _seedSet = false;
    std::size_t _seed    = 0;
    std::string _name;
    std::vector<sf::Color> _palette{ {124, 92, 255}, {58, 160, 224}, {224, 138, 58} };
    sf::Color   _dotColor{ 70, 200, 120 };

    static std::size_t hash(const std::string& s)
    { std::size_t h = 0; for (char c : s) h = h * 31u + (unsigned char)c; return h; }

    void applyColor()
    {
        if (_palette.empty()) return;
        const std::size_t k = (_seedSet ? _seed : hash(_name)) % _palette.size();
        _chip.setFillColor(_palette[k]);
    }

    void relayout()
    {
        _chip.setPosition(_pos);
        _chip.setSize({ _size, _size });
        auto b = _label.getLocalBounds();
        _label.setPosition({ _pos.x + _size * 0.5f - (b.position.x + b.size.x) * 0.5f,
                             _pos.y + _size * 0.5f - _size * 0.30f });
        const float d = std::max(6.f, _size * 0.28f);
        _dot.setSize({ d, d });
        _dot.setPosition({ _pos.x + _size - d, _pos.y + _size - d });
    }

public:
    Avatar()
    {
        _label.setFillColor(sf::Color(235, 238, 245));
        _label.setCharacterSize(11);
        _dot.setFillColor(_dotColor);
        _dot.setVisible(false);
        addComponent(_chip);
        addComponent(_label);
        addComponent(_dot);   // last → draws over the chip corner
        applyColor();
        relayout();
    }

    /// Two-letter initials from a display name (first + last word, else first char).
    static std::string initialsOf(const std::string& name)
    {
        std::string out; bool atStart = true;
        for (char c : name)
        {
            if (c == ' ') { atStart = true; continue; }
            if (atStart && out.size() < 2) { out += (char)std::toupper((unsigned char)c); atStart = false; }
        }
        if (out.empty() && !name.empty()) out += (char)std::toupper((unsigned char)name[0]);
        return out;
    }

    void setName(const std::string& name)
    {
        _name = name;
        _label.setString(initialsOf(name));
        applyColor();
        relayout();
    }
    /// Force the tile colour by seed instead of the name hash.
    void setColorSeed(std::size_t seed) { _seed = seed; _seedSet = true; applyColor(); }
    void setPalette(std::vector<sf::Color> palette) { _palette = std::move(palette); applyColor(); }
    void setAvatarSize(float px)          { _size = px; _label.setCharacterSize((unsigned)(px * 0.42f)); relayout(); }
    void setInitialsColor(const sf::Color& c) { _label.setFillColor(c); }
    /// Show/hide the corner presence dot.
    void setPresence(bool present)        { _showDot = present; _dot.setVisible(present); }
    void setPresenceColor(const sf::Color& c) { _dotColor = c; _dot.setFillColor(c); }

    void setPosition(const sf::Vector2f& position) override { _pos = position; relayout(); }
    sf::Vector2f  getPosition()     const override { return _pos; }
    sf::FloatRect getGlobalBounds() const override { return sf::FloatRect{ _pos, { _size, _size } }; }
};

} // namespace ml

#endif // MALENA_AVATAR_H
