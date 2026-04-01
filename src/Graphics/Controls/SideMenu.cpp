// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/SideMenu.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <algorithm>

namespace ml
{
    SideMenu::SideMenu(Mode mode_, Anchor anchor_, const sf::Font& font_)
        : _defaultList(font_),
          _content(&_defaultList)
    {
        SideMenuTheme::applyFrom(ThemeManager::get());
        this->font = &font_;
        this->mode   = mode_;
        this->anchor = anchor_;

        _panel.setFillColor(panelBg);
        _backdrop.setFillColor(backdropColor);

        // Start in closed state (off-screen)
        _currentX = closedX();
        _targetX  = _currentX;
        setState(State::CLOSED);

        // ── Per-frame: animation + backdrop click detection ───────────────────
        onUpdate([this]{
            // Animate panel
            if (_animating)
            {
                const float diff = _targetX - _currentX;
                if (std::abs(diff) < 0.5f)
                {
                    _currentX  = _targetX;
                    _animating = false;
                    setState(checkFlag(Flag::OPEN) ? State::OPEN : State::CLOSED);
                    if (checkFlag(Flag::OPEN) && _onOpenCb)  _onOpenCb();
                    if (!checkFlag(Flag::OPEN) && _onCloseCb) _onCloseCb();
                }
                else
                {
                    // Exponential ease-out
                    _currentX += diff * 0.18f;
                }
                updatePanelTransform();
            }

            // OVERLAY mode: click outside panel → close
            if (checkFlag(Flag::OPEN) && mode == Mode::OVERLAY)
            {
                static bool prevDown = false;
                const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                if (down && !prevDown)
                {
                    const sf::Vector2f wp =
                        WindowManager::getWindow().mapPixelToCoords(
                            sf::Mouse::getPosition(WindowManager::getWindow()));
                    const sf::FloatRect panelBounds{
                        {_currentX, _hamburgerPos.y},
                        {panelWidth, resolvedHeight()}
                    };
                    const sf::FloatRect hamburgerBounds{
                        _hamburgerPos, {hamburgerSize, hamburgerSize}
                    };
                    if (!panelBounds.contains(wp) && !hamburgerBounds.contains(wp))
                        close();
                }
                prevDown = down;
            }
        });

        // ── Hamburger click ───────────────────────────────────────────────────
        this->onClick([this]{
            const sf::Vector2f wp =
                WindowManager::getWindow().mapPixelToCoords(
                    sf::Mouse::getPosition(WindowManager::getWindow()));
            const sf::FloatRect hb{_hamburgerPos, {hamburgerSize, hamburgerSize}};
            if (hb.contains(wp))
                toggle();
        });

        updatePanelTransform();
    }

    // ── Geometry helpers ──────────────────────────────────────────────────────

    float SideMenu::closedX() const
    {
        if (anchor == Anchor::LEFT)
            return -panelWidth;
        else
            return static_cast<float>(WindowManager::getWindow().getSize().x);
    }

    float SideMenu::openX() const
    {
        if (anchor == Anchor::LEFT)
            return 0.f;
        else
            return static_cast<float>(WindowManager::getWindow().getSize().x) - panelWidth;
    }

    float SideMenu::resolvedHeight() const
    {
        return panelHeight > 0.f
               ? panelHeight
               : static_cast<float>(WindowManager::getWindow().getSize().y);
    }

    void SideMenu::updatePanelTransform()
    {
        const float h = resolvedHeight();
        _panel.setSize({panelWidth, h});
        _panel.setPosition({_currentX, 0.f});

        // Position backdrop to cover full window
        const auto& win = WindowManager::getWindow();
        _backdrop.setSize({static_cast<float>(win.getSize().x),
                           static_cast<float>(win.getSize().y)});
        _backdrop.setPosition({0.f, 0.f});

        // Reposition content inside panel
        if (_content)
        {
            const float contentX = _currentX + padding;
            const float contentY = hamburgerSize + padding;
            _content->setPosition({contentX, contentY});

            // Resize list width to fit panel
            if (auto* list = dynamic_cast<ml::List*>(_content))
                list->setWidth(panelWidth - padding * 2.f);
        }
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void SideMenu::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        SideMenuTheme::applyFrom(theme);
        _panel.setFillColor(panelBg);
        _backdrop.setFillColor(backdropColor);
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void SideMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Hamburger button (always visible)
        drawHamburger(target, states);

        // Nothing else when panel is fully closed and not animating
        if (!checkFlag(Flag::OPEN) && !_animating) return;

        // OVERLAY backdrop
        if (mode == Mode::OVERLAY)
        {
            const float alpha = std::clamp(
                std::abs(_currentX - closedX()) / panelWidth, 0.f, 1.f);
            sf::RectangleShape bd = _backdrop;
            sf::Color c = bd.getFillColor();
            c.a = static_cast<std::uint8_t>(backdropColor.a * alpha);
            bd.setFillColor(c);
            target.draw(bd, states);
        }

        // Panel background - ensure color is always opaque
        sf::RectangleShape visiblePanel = _panel;
        sf::Color pc = _panel.getFillColor();
        if (pc.a < 200) pc.a = 245;
        visiblePanel.setFillColor(pc);
        target.draw(visiblePanel, states);

        // Content
        if (_content)
            target.draw(*dynamic_cast<const sf::Drawable*>(_content), states);
    }

    void SideMenu::drawHamburger(sf::RenderTarget& target,
                                  const sf::RenderStates& states) const
    {
        // Background — always a solid dark pill so it's visible over any content
        {
            sf::RectangleShape bg({hamburgerSize, hamburgerSize});
            bg.setFillColor(sf::Color(20, 20, 28, 220));
            bg.setPosition(_hamburgerPos);
            target.draw(bg, states);
        }

        // 3 horizontal lines — guaranteed white so they're always visible
        const float lineW  = hamburgerSize * 0.58f;
        const float lineH  = 3.f;
        const float startX = _hamburgerPos.x + (hamburgerSize - lineW) / 2.f;
        const float spacing = (hamburgerSize - hamburgerSize * 0.26f * 2.f) / 2.f;
        const float topY   = _hamburgerPos.y + hamburgerSize * 0.26f;

        for (int i = 0; i < 3; ++i)
        {
            sf::RectangleShape line({lineW, lineH});
            // Use hamburgerColor if set, otherwise white
            const sf::Color lineCol = (hamburgerColor.r == 0 &&
                                       hamburgerColor.g == 0 &&
                                       hamburgerColor.b == 0)
                                      ? sf::Color::White : hamburgerColor;
            line.setFillColor(lineCol);
            line.setPosition({startX, topY + static_cast<float>(i) * spacing});
            target.draw(line, states);
        }
    }

    // ── Content ───────────────────────────────────────────────────────────────

    ml::List& SideMenu::getList()
    {
        _content = &_defaultList;
        return _defaultList;
    }

    void SideMenu::setContent(ml::Core& c)
    {
        _content = &c;
        updatePanelTransform();
    }

    // ── Control ───────────────────────────────────────────────────────────────

    void SideMenu::open()
    {
        if (checkFlag(Flag::OPEN)) return;
        enableFlag(Flag::OPEN);
        setState(State::ANIMATING);
        _targetX  = openX();
        _animating = true;
    }

    void SideMenu::close()
    {
        if (!checkFlag(Flag::OPEN) && !_animating) return;
        disableFlag(Flag::OPEN);
        setState(State::ANIMATING);
        _targetX  = closedX();
        _animating = true;
    }

    void SideMenu::toggle() { isOpen() ? close() : open(); }
    bool SideMenu::isOpen() const { return checkFlag(Flag::OPEN); }

    void SideMenu::onOpen(std::function<void()> cb)  { _onOpenCb  = std::move(cb); }
    void SideMenu::onClose(std::function<void()> cb) { _onCloseCb = std::move(cb); }

    // ── Positioning ───────────────────────────────────────────────────────────

    void SideMenu::setHamburgerPosition(const sf::Vector2f& pos)
    {
        _hamburgerPos = pos;
        updatePanelTransform();
    }

    void SideMenu::setPosition(const sf::Vector2f&) {}  // position is window-relative

    sf::Vector2f SideMenu::getPosition() const
    {
        return _hamburgerPos;
    }

    sf::FloatRect SideMenu::getGlobalBounds() const
    {
        // When open in OVERLAY mode cover the full window to catch backdrop clicks
        if (checkFlag(Flag::OPEN) && mode == Mode::OVERLAY)
        {
            const auto& win = WindowManager::getWindow();
            return sf::FloatRect{{0.f, 0.f},
                {static_cast<float>(win.getSize().x),
                 static_cast<float>(win.getSize().y)}};
        }
        // Otherwise just the hamburger button area
        return sf::FloatRect{_hamburgerPos, {hamburgerSize, hamburgerSize}};
    }

} // namespace ml
