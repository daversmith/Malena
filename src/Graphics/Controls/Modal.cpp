// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Controls/Modal.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    Modal::Modal(const sf::Font& font)
        : _titleText(font),
          _confirmText(font),
          _cancelText(font),
          _closeText(font)
    {
        ModalTheme::applyFrom(ThemeManager::get());

        _titleText.setCharacterSize(16);
        _confirmText.setCharacterSize(14);
        _cancelText.setCharacterSize(14);
        _closeText.setCharacterSize(18);
        _closeText.setString("x");

        setState(State::HIDDEN);

        // ── Per-frame: fade animation + backdrop click detection ──────────────
        onUpdate([this]
        {
            if (_animating)
            {
                const float target = _fadingIn ? 255.f : 0.f;
                const float diff   = target - _alpha;

                if (std::abs(diff) < 1.f)
                {
                    _alpha     = target;
                    _animating = false;
                    if (!_fadingIn)
                    {
                        disableFlag(Flag::VISIBLE);
                        setState(State::HIDDEN);
                        if (_onDismiss) _onDismiss();
                    }
                    else
                    {
                        setState(State::VISIBLE);
                    }
                }
                else
                {
                    _alpha += diff * 0.2f;
                }
            }

            // Backdrop or close-button click while visible
            if (checkFlag(Flag::VISIBLE) || _animating)
            {
                static bool prevDown = false;
                const bool  down     = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

                if (down && !prevDown)
                {
                    const auto& win = WindowManager::getWindow();
                    const sf::Vector2f wp =
                        win.mapPixelToCoords(sf::Mouse::getPosition(win));

                    if (showCloseButton && _closeBounds.contains(wp))
                    {
                        hide();
                    }
                    else if (hasButtons() && _confirmBounds.contains(wp))
                    {
                        hide();
                        if (_onConfirm) _onConfirm();
                    }
                    else if (hasButtons() && _cancelBounds.contains(wp))
                    {
                        hide();
                    }
                    else if (dismissOnBackdrop && !_panelBounds.contains(wp))
                    {
                        hide();
                    }
                }
                prevDown = down;
            }
        });
    }

    // ── Geometry helpers ──────────────────────────────────────────────────────

    sf::Vector2f Modal::panelOrigin() const
    {
        const auto& win = WindowManager::getWindow();
        return {
            (static_cast<float>(win.getSize().x) - modalSize.x) * 0.5f,
            (static_cast<float>(win.getSize().y) - modalSize.y) * 0.5f
        };
    }

    bool Modal::hasButtons() const
    {
        return !_confirmStr.empty();
    }

    float Modal::contentTop() const
    {
        return _titleStr.empty() ? padding : titleBarHeight;
    }

    float Modal::contentHeight() const
    {
        const float bottom = hasButtons()
            ? modalSize.y - buttonBarHeight
            : modalSize.y - padding;
        return bottom - contentTop();
    }

    void Modal::applyLayout()
    {
        const sf::Vector2f origin = panelOrigin();

        _panelBounds = {origin, modalSize};

        // ── Backdrop ─────────────────────────────────────────────────────────
        const auto& win = WindowManager::getWindow();
        _backdrop.setSize({static_cast<float>(win.getSize().x),
                           static_cast<float>(win.getSize().y)});
        _backdrop.setPosition({0.f, 0.f});

        // ── Panel ─────────────────────────────────────────────────────────────
        _panel.setSize(modalSize);
        _panel.setPosition(origin);

        // ── Title separator ───────────────────────────────────────────────────
        if (!_titleStr.empty())
        {
            _titleText.setPosition(
                {origin.x + padding, origin.y + (titleBarHeight - 18.f) * 0.5f});

            _separator.setSize({modalSize.x, 1.f});
            _separator.setPosition({origin.x, origin.y + titleBarHeight});
        }

        // ── Close button (top-right "x") ──────────────────────────────────────
        if (showCloseButton)
        {
            constexpr float closeSz = 24.f;
            const float cx = origin.x + modalSize.x - closeSz - padding * 0.5f;
            const float cy = origin.y + (titleBarHeight - closeSz) * 0.5f;
            _closeBounds = {{cx, cy}, {closeSz, closeSz}};
            _closeText.setPosition({cx + 4.f, cy});
        }

        // ── Button bar ────────────────────────────────────────────────────────
        if (hasButtons())
        {
            const float barY    = origin.y + modalSize.y - buttonBarHeight;
            const float btnH    = buttonBarHeight - padding;
            const float btnY    = barY + padding * 0.5f;
            const bool  hasCancel = !_cancelStr.empty();

            if (hasCancel)
            {
                const float btnW  = (modalSize.x - padding * 3.f) * 0.5f;
                _cancelBounds  = {{origin.x + padding, btnY}, {btnW, btnH}};
                _confirmBounds = {{origin.x + padding * 2.f + btnW, btnY}, {btnW, btnH}};
            }
            else
            {
                const float btnW  = modalSize.x - padding * 2.f;
                _confirmBounds = {{origin.x + padding, btnY}, {btnW, btnH}};
            }

            _confirmShape.setSize(_confirmBounds.size);
            _confirmShape.setPosition(_confirmBounds.position);

            _confirmText.setString(_confirmStr);
            const auto cb = _confirmText.getGlobalBounds();
            _confirmText.setPosition({
                _confirmBounds.position.x + (_confirmBounds.size.x - cb.size.x) * 0.5f,
                _confirmBounds.position.y + (_confirmBounds.size.y - cb.size.y) * 0.5f - 2.f
            });

            if (hasCancel)
            {
                _cancelShape.setSize(_cancelBounds.size);
                _cancelShape.setPosition(_cancelBounds.position);

                _cancelText.setString(_cancelStr);
                const auto kb = _cancelText.getGlobalBounds();
                _cancelText.setPosition({
                    _cancelBounds.position.x + (_cancelBounds.size.x - kb.size.x) * 0.5f,
                    _cancelBounds.position.y + (_cancelBounds.size.y - kb.size.y) * 0.5f - 2.f
                });
            }
        }

        // ── Content ───────────────────────────────────────────────────────────
        if (_content)
        {
            const auto& win    = WindowManager::getWindow();
            const auto  bounds = _content->getGlobalBounds();
            _content->setPosition({
                (static_cast<float>(win.getSize().x) - bounds.size.x) * 0.5f,
                (static_cast<float>(win.getSize().y) - bounds.size.y) * 0.5f
            });
        }
    }

    // ── Alpha helper ──────────────────────────────────────────────────────────

    void Modal::applyAlphaToColor(sf::Color& c, float alpha) const
    {
        c.a = static_cast<std::uint8_t>(
            static_cast<float>(c.a) * std::clamp(alpha / 255.f, 0.f, 1.f));
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void Modal::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!checkFlag(Flag::VISIBLE) && !_animating) return;

        const float a = std::clamp(_alpha / 255.f, 0.f, 1.f);

        // Backdrop
        {
            sf::RectangleShape bd = _backdrop;
            sf::Color c = backdropColor;
            c.a = static_cast<std::uint8_t>(static_cast<float>(c.a) * a);
            bd.setFillColor(c);
            target.draw(bd, states);
        }

        // Panel
        {
            sf::RectangleShape p = _panel;
            sf::Color c = panelColor;
            c.a = static_cast<std::uint8_t>(255.f * a);
            p.setFillColor(c);
            target.draw(p, states);
        }

        // Title bar
        if (!_titleStr.empty())
        {
            sf::Text t = static_cast<const sf::Text&>(_titleText);
            sf::Color c = titleTextColor;
            c.a = static_cast<std::uint8_t>(255.f * a);
            t.setFillColor(c);
            target.draw(t, states);

            sf::RectangleShape sep = _separator;
            sf::Color sc = separatorColor;
            sc.a = static_cast<std::uint8_t>(255.f * a);
            sep.setFillColor(sc);
            target.draw(sep, states);
        }

        // Close button
        if (showCloseButton)
        {
            sf::Text ct = static_cast<const sf::Text&>(_closeText);
            sf::Color c = closeBtnColor;
            c.a = static_cast<std::uint8_t>(255.f * a);
            ct.setFillColor(c);
            target.draw(ct, states);
        }

        // Content
        if (_content)
        {
            if (auto* d = dynamic_cast<const sf::Drawable*>(_content))
                target.draw(*d, states);
        }

        // Button bar
        if (hasButtons())
        {
            // Confirm
            {
                sf::RectangleShape btn = _confirmShape;
                sf::Color c = confirmColor;
                c.a = static_cast<std::uint8_t>(255.f * a);
                btn.setFillColor(c);
                target.draw(btn, states);

                sf::Text lbl = static_cast<const sf::Text&>(_confirmText);
                sf::Color tc = btnTextColor;
                tc.a = static_cast<std::uint8_t>(255.f * a);
                lbl.setFillColor(tc);
                target.draw(lbl, states);
            }

            // Cancel
            if (!_cancelStr.empty())
            {
                sf::RectangleShape btn = _cancelShape;
                sf::Color c = cancelColor;
                c.a = static_cast<std::uint8_t>(255.f * a);
                btn.setFillColor(c);
                target.draw(btn, states);

                sf::Text lbl = static_cast<const sf::Text&>(_cancelText);
                sf::Color tc = btnTextColor;
                tc.a = static_cast<std::uint8_t>(255.f * a);
                lbl.setFillColor(tc);
                target.draw(lbl, states);
            }
        }
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void Modal::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        ModalTheme::applyFrom(theme);
    }

    // ── Content setters ───────────────────────────────────────────────────────

    void Modal::setTitle(const std::string& title)
    {
        _titleStr = title;
        _titleText.setString(title);
        _titleText.setFillColor(titleTextColor);
        applyLayout();
    }

    void Modal::setContent(ml::Core& content)
    {
        _content = &content;
        applyLayout();
    }

    void Modal::setConfirmLabel(const std::string& label)
    {
        _confirmStr = label;
        applyLayout();
    }

    void Modal::setCancelLabel(const std::string& label)
    {
        _cancelStr = label;
        applyLayout();
    }

    // ── Control ───────────────────────────────────────────────────────────────

    void Modal::show()
    {
        if (checkFlag(Flag::VISIBLE) && !_animating) return;
        enableFlag(Flag::VISIBLE);
        setState(State::ANIMATING);
        applyLayout();
        _fadingIn  = true;
        _animating = true;
    }

    void Modal::hide()
    {
        if (!checkFlag(Flag::VISIBLE) && !_animating) return;
        setState(State::ANIMATING);
        _fadingIn  = false;
        _animating = true;
    }

    void Modal::toggle() { isVisible() ? hide() : show(); }
    bool Modal::isVisible() const { return checkFlag(Flag::VISIBLE); }

    void Modal::onConfirm(std::function<void()> cb) { _onConfirm = std::move(cb); }
    void Modal::onDismiss(std::function<void()> cb) { _onDismiss = std::move(cb); }

    // ── Positioning ───────────────────────────────────────────────────────────

    void Modal::setPosition(const sf::Vector2f&) {}  // always window-centered

    sf::Vector2f Modal::getPosition() const
    {
        return panelOrigin();
    }

    sf::FloatRect Modal::getGlobalBounds() const
    {
        if (!checkFlag(Flag::VISIBLE) && !_animating)
            return {{-1.f, -1.f}, {0.f, 0.f}};

        const auto& win = WindowManager::getWindow();
        return {{0.f, 0.f},
                {static_cast<float>(win.getSize().x),
                 static_cast<float>(win.getSize().y)}};
    }

} // namespace ml
