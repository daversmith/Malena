// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    ButtonToggle::ButtonToggle()
    {
        syncFromSettings();

        onHover([this]{ if (!checkFlag(Flag::DISABLED)) setState(State::HOVERED); });
        onUnhover([this]{ if (!checkFlag(Flag::DISABLED)) setState(checkFlag(Flag::ON) ? State::ON : State::IDLE); });
        onClick([this]{ if (!checkFlag(Flag::DISABLED)) toggle(); });
    }

    void ButtonToggle::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        ButtonSettings::applyTheme(theme);
        syncFromSettings();
    }

    void ButtonToggle::syncFromSettings() {}

    void ButtonToggle::drawCapsule(sf::RenderTarget&       target,
                                    const sf::RenderStates& states,
                                    sf::Vector2f pos, sf::Vector2f sz,
                                    float r, sf::Color fill) const
    {
        r = std::min(r, sz.y / 2.f);
        sf::CircleShape cap(r);
        cap.setFillColor(fill);
        cap.setOrigin({r, r});
        cap.setPosition({pos.x + r, pos.y + r});
        target.draw(cap, states);
        cap.setPosition({pos.x + sz.x - r, pos.y + r});
        target.draw(cap, states);
        sf::RectangleShape mid({sz.x - r * 2.f, sz.y});
        mid.setFillColor(fill);
        mid.setPosition({pos.x + r, pos.y});
        target.draw(mid, states);
    }

    void ButtonToggle::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const bool on      = checkFlag(Flag::ON);
        const bool dis     = checkFlag(Flag::DISABLED);
        const sf::Color bg = dis ? disabledColor : (on ? onColor : offColor);
        const sf::Color tc = dis ? disabledTextColor : (on ? onTextColor : offTextColor);
        const std::string& str = on ? onLabel : offLabel;
        const sf::Texture* iconTex = on ? iconOn : iconOff;
        if (!iconTex) iconTex = iconOn ? iconOn : iconOff;

        // Background — plain rectangle with optional outline
        sf::RectangleShape btn(size);
        btn.setPosition(_position);
        btn.setFillColor(bg);
        btn.setOutlineColor(borderThickness > 0.f ? borderColor : sf::Color::Transparent);
        btn.setOutlineThickness(borderThickness);
        target.draw(btn, states);

        // Content
        const bool hasIcon = (iconTex != nullptr);
        const bool hasText = !str.empty() && iconAlign != ButtonSettings::IconAlign::ICON_ONLY;

        sf::Vector2f iSize = iconSize;
        if (hasIcon && iSize.x <= 0.f)
        {
            const sf::Vector2u ts = iconTex->getSize();
            iSize = {static_cast<float>(ts.x), static_cast<float>(ts.y)};
        }

        const float cx = _position.x + size.x / 2.f;
        const float cy = _position.y + size.y / 2.f;

        sf::Text label(*font, str, fontSize);
        label.setFillColor(tc);
        const sf::FloatRect lb = label.getLocalBounds();

        if (hasIcon && !hasText)
        {
            sf::Sprite sprite(*iconTex);
            const sf::Vector2u ts = iconTex->getSize();
            sprite.setScale({iSize.x / ts.x, iSize.y / ts.y});
            sprite.setPosition({cx - iSize.x / 2.f, cy - iSize.y / 2.f});
            target.draw(sprite, states);
        }
        else if (hasIcon && hasText)
        {
            sf::Sprite sprite(*iconTex);
            const sf::Vector2u ts = iconTex->getSize();
            sprite.setScale({iSize.x / ts.x, iSize.y / ts.y});
            const float pad = iconPadding;

            if (iconAlign == ButtonSettings::IconAlign::ICON_LEFT)
            {
                const float totalW = iSize.x + pad + lb.size.x;
                const float sx = cx - totalW / 2.f;
                sprite.setPosition({sx, cy - iSize.y / 2.f});
                label.setPosition({sx + iSize.x + pad - lb.position.x,
                                   cy - lb.size.y / 2.f - lb.position.y});
            }
            else if (iconAlign == ButtonSettings::IconAlign::ICON_RIGHT)
            {
                const float totalW = lb.size.x + pad + iSize.x;
                const float sx = cx - totalW / 2.f;
                label.setPosition({sx - lb.position.x, cy - lb.size.y / 2.f - lb.position.y});
                sprite.setPosition({sx + lb.size.x + pad, cy - iSize.y / 2.f});
            }
            else // ICON_ABOVE
            {
                const float totalH = iSize.y + pad + lb.size.y;
                const float sy = cy - totalH / 2.f;
                sprite.setPosition({cx - iSize.x / 2.f, sy});
                label.setPosition({cx - lb.size.x / 2.f - lb.position.x,
                                   sy + iSize.y + pad - lb.position.y});
            }
            target.draw(sprite, states);
            target.draw(label, states);
        }
        else
        {
            label.setPosition({
                cx - lb.size.x / 2.f - lb.position.x,
                cy - lb.size.y / 2.f - lb.position.y
            });
            target.draw(label, states);
        }
    }

    void ButtonToggle::setOn(bool on)
    {
        if (on == checkFlag(Flag::ON)) return;
        if (on) { enableFlag(Flag::ON);  setState(State::ON); }
        else    { disableFlag(Flag::ON); setState(State::IDLE); }
        if (_onToggled) _onToggled(on);
    }

    void ButtonToggle::toggle()  { setOn(!checkFlag(Flag::ON)); }
    bool ButtonToggle::isOn() const { return checkFlag(Flag::ON); }

    void ButtonToggle::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
    }

    bool ButtonToggle::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void ButtonToggle::setSize(const sf::Vector2f& s)          { size = s; }
    sf::Vector2f ButtonToggle::getSize() const                 { return size; }
    void ButtonToggle::setButtonOnLabel(const std::string& l)  { onLabel = l; }
    void ButtonToggle::setButtonOffLabel(const std::string& l) { offLabel = l; }
    void ButtonToggle::setButtonOnColor(const sf::Color& c)    { onColor = c; }
    void ButtonToggle::setButtonOffColor(const sf::Color& c)   { offColor = c; }
    void ButtonToggle::setButtonOnTextColor(const sf::Color& c){ onTextColor = c; }
    void ButtonToggle::setButtonOffTextColor(const sf::Color& c){offTextColor = c; }
    void ButtonToggle::setBorderColor(const sf::Color& c)      { borderColor = c; }
    void ButtonToggle::setBorderThickness(float t)             { borderThickness = t; }
    void ButtonToggle::setButtonRadius(float r)                { buttonRadius = r; }
    void ButtonToggle::setIcon(const sf::Texture* on, const sf::Texture* off)
    { iconOn = on; iconOff = off ? off : on; }
    void ButtonToggle::setIconSize(const sf::Vector2f& s)      { iconSize = s; }
    void ButtonToggle::setIconAlign(ButtonSettings::IconAlign a){ iconAlign = a; }
    void ButtonToggle::setIconPadding(float p)                 { iconPadding = p; }
    void ButtonToggle::setFont(const sf::Font& f)              { font = &f; }
    void ButtonToggle::setCharacterSize(unsigned int s)        { fontSize = s; }
    void ButtonToggle::onToggled(std::function<void(bool)> cb) { _onToggled = std::move(cb); }

    void ButtonToggle::setPosition(const sf::Vector2f& pos)
    { _position = {std::round(pos.x), std::round(pos.y)}; }

    sf::Vector2f  ButtonToggle::getPosition()     const { return _position; }
    sf::FloatRect ButtonToggle::getGlobalBounds() const
    { return sf::FloatRect{_position, size}; }

} // namespace ml