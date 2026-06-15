// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Controls/InfoButton.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Engine/App/AppManager.h>   // exclusive-owner (modal) input gating

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <sstream>

namespace ml
{
    namespace
    {
        // Greedily wrap `text` to `maxWidth` for the given font/size.
        std::string wrap(const sf::Font& font, unsigned int size, float maxWidth,
                         const std::string& text)
        {
            sf::Text probe(font, "", size);
            std::string out, line;
            std::stringstream paras(text);
            std::string para;
            bool firstPara = true;
            while (std::getline(paras, para))            // preserve explicit newlines
            {
                if (!firstPara) out += '\n';
                firstPara = false;
                line.clear();
                std::stringstream words(para);
                std::string w;
                while (words >> w)
                {
                    const std::string trial = line.empty() ? w : line + " " + w;
                    probe.setString(trial);
                    if (probe.getLocalBounds().size.x > maxWidth && !line.empty())
                    {
                        out += line + "\n";
                        line = w;
                    }
                    else line = trial;
                }
                out += line;
            }
            return out;
        }
    }

    InfoButton::InfoButton(const sf::Font& font) : _font(&font)
    {
        this->setFillColor(sf::Color::Transparent);

        onUpdate([this] {
            // Skip when hidden/disabled or a modal owns input (no click-through).
            if (checkFlag(ml::Flag::HIDDEN) || !checkFlag(ml::Flag::ENABLED) ||
                !AppManager::isUnderExclusiveOwner(this))
            {
                _prevDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                return;
            }

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            if (down && !_prevDown)
            {
                const sf::FloatRect badge{ _pos, { _radius * 2.f, _radius * 2.f } };
                if (badge.contains(wp)) toggle();
                else if (_open)         close();   // click elsewhere dismisses
            }
            _prevDown = down;
        });
    }

    void InfoButton::setText(const std::string& help) { _text = help; }
    void InfoButton::setRadius(float r)               { _radius = r; }
    void InfoButton::setPopoverWidth(float w)         { _popWidth = w; }

    void InfoButton::open()   { _open = true; }
    void InfoButton::close()  { _open = false; }
    void InfoButton::toggle() { _open = !_open; }

    void InfoButton::setPosition(const sf::Vector2f& pos) { _pos = pos; }
    sf::Vector2f  InfoButton::getPosition()     const { return _pos; }
    sf::FloatRect InfoButton::getGlobalBounds() const
    { return { _pos, { _radius * 2.f, _radius * 2.f } }; }

    void InfoButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // ── Badge ───────────────────────────────────────────────────────────
        sf::CircleShape badge(_radius);
        badge.setPosition(_pos);
        badge.setFillColor(_open ? sf::Color(70, 110, 200) : sf::Color(90, 96, 110));
        badge.setOutlineThickness(1.f);
        badge.setOutlineColor(sf::Color(150, 155, 165));
        target.draw(badge, states);

        if (_font)
        {
            sf::Text glyph(*_font, "i", static_cast<unsigned int>(_radius * 1.4f));
            glyph.setStyle(sf::Text::Bold);
            glyph.setFillColor(sf::Color::White);
            const sf::FloatRect gb = glyph.getLocalBounds();
            glyph.setPosition({ _pos.x + _radius - gb.size.x / 2.f - gb.position.x,
                                _pos.y + _radius - gb.size.y / 2.f - gb.position.y });
            target.draw(glyph, states);
        }

        // ── Popover ─────────────────────────────────────────────────────────
        if (_open && _font && !_text.empty())
        {
            const unsigned int cs  = 13;
            const float pad = 8.f;
            sf::Text body(*_font, wrap(*_font, cs, _popWidth - 2.f * pad, _text), cs);
            body.setFillColor(sf::Color(230, 232, 238));
            const sf::FloatRect bb = body.getLocalBounds();

            const float w = _popWidth;
            const float h = bb.size.y + 2.f * pad + 4.f;
            const sf::Vector2f at{ _pos.x, _pos.y + _radius * 2.f + 4.f };

            sf::RectangleShape box({ w, h });
            box.setPosition(at);
            box.setFillColor(sf::Color(34, 37, 44));
            box.setOutlineThickness(1.f);
            box.setOutlineColor(sf::Color(70, 110, 200));
            target.draw(box, states);

            body.setPosition({ at.x + pad - bb.position.x, at.y + pad - bb.position.y });
            target.draw(body, states);
        }
    }

} // namespace ml
