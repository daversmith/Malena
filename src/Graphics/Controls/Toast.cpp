// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Controls/Toast.h>
#include <Malena/Utilities/Utf8.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <algorithm>

namespace ml
{
    Toast::Toast()
    {
        _bg.setFillColor(sf::Color(40, 44, 52));
        _bg.setOutlineThickness(1.f);
        _bg.setOutlineColor(sf::Color(80, 84, 92));

        _message.setCharacterSize(15);
        _message.setFillColor(sf::Color::White);

        _action.setString("Connect");
        _action.setFillColor(sf::Color(60, 120, 200));

        _dismiss.setString("Dismiss");
        _dismiss.setFillColor(sf::Color(70, 74, 82));

        _action.onClick ([this] { hide(); if (_onAction)  _onAction();  });
        _dismiss.onClick([this] { hide(); if (_onDismiss) _onDismiss(); });

        // Registered for setEnabled propagation; gated off while hidden so the
        // buttons never intercept clicks before the toast is shown.
        addComponent(_action);
        addComponent(_dismiss);
        _action.setEnabled(false);
        _dismiss.setEnabled(false);
    }

    void Toast::setMessage(const std::string& text)
    {
        _message.setString(ml::utf8(text));
        if (_visible) layout();
    }

    void Toast::setActionLabel(const std::string& label)
    {
        _hasAction = !label.empty();
        if (_hasAction) _action.setString(ml::utf8(label));
        if (_visible)
        {
            _action.setEnabled(_hasAction);
            layout();
        }
    }

    void Toast::show()
    {
        _visible = true;
        layout();
        _action.setEnabled(_hasAction);
        _dismiss.setEnabled(true);
    }

    void Toast::hide()
    {
        _visible = false;
        _action.setEnabled(false);
        _dismiss.setEnabled(false);
    }

    void Toast::layout()
    {
        const sf::Vector2f win = (sf::Vector2f)WindowManager::getWindow().getSize();

        constexpr float pad = 16.f, gap = 12.f, h = 56.f;
        constexpr float actionW = 110.f, dismissW = 90.f, btnH = 34.f;

        const float msgW = _message.getGlobalBounds().size.x;
        float w = pad + msgW + gap
                + (_hasAction ? actionW + gap : 0.f)
                + dismissW + pad;
        w = std::min(w, win.x - 40.f);

        const float x = (win.x - w) / 2.f;
        const float y = (_anchor == Anchor::BOTTOM) ? win.y - h - 20.f : 20.f;

        _bg.setSize({w, h});
        _bg.setPosition({x, y});

        _message.setPosition({x + pad,
                              y + (h - _message.getGlobalBounds().size.y) / 2.f - 4.f});

        float bx = x + w - pad - dismissW;
        _dismiss.setSize({dismissW, btnH});
        _dismiss.setPosition({bx, y + (h - btnH) / 2.f});

        if (_hasAction)
        {
            bx -= gap + actionW;
            _action.setSize({actionW, btnH});
            _action.setPosition({bx, y + (h - btnH) / 2.f});
        }
    }

    void Toast::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!_visible) return;
        target.draw(_bg, states);
        target.draw(_message, states);
        if (_hasAction) target.draw(_action, states);
        target.draw(_dismiss, states);
    }

    void Toast::setPosition(const sf::Vector2f&) {}   // anchored to the window; layout() places it

    sf::Vector2f Toast::getPosition() const
    {
        return _bg.getPosition();
    }

    sf::FloatRect Toast::getGlobalBounds() const
    {
        return _bg.getGlobalBounds();
    }

} // namespace ml
