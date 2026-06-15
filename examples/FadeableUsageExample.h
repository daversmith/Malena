#pragma once
// ============================================================
//  FadeableUsageExample.h
// ============================================================
//
//  Shows three different usage patterns for the Fadeable trait.
//
// ============================================================

#include <Malena/Core/Component.h>
#include <Malena/Traits/Fadeable.h>
#include <SFML/Graphics.hpp>

// ── Example 1: Simple panel that fades in on construction ─────────────────────
//
//  Mixing Fadeable in as a Component template argument is all that's needed.
//  The trait self-subscribes to "update" in its constructor — no onUpdate()
//  call required.

class FadePanel : public ml::Component<ml::Fadeable>
{
public:
    FadePanel()
    {
        _body.setSize({ 200.f, 100.f });
        _body.setFillColor(sf::Color(60, 80, 180));

        setAlpha(0);           // start invisible
        fadeIn(0.6f);          // animate to full opacity over 0.6 seconds
    }

    void setPosition(const sf::Vector2f& pos) override { _body.setPosition(pos); }
    sf::Vector2f  getPosition()     const override { return _body.getPosition(); }
    sf::FloatRect getGlobalBounds() const override { return _body.getGlobalBounds(); }

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
    {
        // Apply the animated alpha from the trait before drawing
        sf::Color c = _body.getFillColor();
        c.a = getAlpha();
        _body.setFillColor(c);
        target.draw(_body, states);
    }

private:
    mutable sf::RectangleShape _body;
};


// ── Example 2: Chained sequence — fade in, hold, fade out ─────────────────────
//
//  Completion callbacks let you chain animations without timers or state flags.

class ToastMessage : public ml::Component<ml::Fadeable>
{
public:
    ToastMessage(const sf::Font& font, const std::string& text)
        : _text(font, text, 18)
    {
        _text.setFillColor(sf::Color::White);

        // Fade in → hold 2 seconds → fade out, all in one chain
        setAlpha(0);
        fadeIn(0.3f, ml::LINEAR, [this]
        {
            holdFor(2.0f, [this]
            {
                fadeOut(0.5f, ml::EXPONENTIAL);
            });
        });
    }

    void setPosition(const sf::Vector2f& pos) override { _text.setPosition(pos); }
    sf::Vector2f  getPosition()     const override { return _text.getPosition(); }
    sf::FloatRect getGlobalBounds() const override { return _text.getGlobalBounds(); }

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
    {
        sf::Color c = _text.getFillColor();
        c.a = getAlpha();
        _text.setFillColor(c);
        target.draw(_text, states);
    }

private:
    mutable sf::Text _text;
};


// ── Example 3: EventLog entry rewritten using Fadeable ────────────────────────
//
//  Before: each Entry carried its own age/alpha int members and the onUpdate
//  loop decremented alpha manually using raw frame counts.
//
//  After: each entry is a small Component<Fadeable>. The timing logic
//  disappears from EventLog entirely — just call holdFor().then(fadeOut()).

class LogEntry : public ml::Component<ml::Fadeable>
{
public:
    LogEntry(const sf::Font& font, const std::string& text,
             sf::Color color, sf::Vector2f position)
        : _text(font, text, 12)
    {
        _text.setFillColor(color);
        setPosition(position);

        // Hold 3 seconds at full opacity, then fade out over 1 second
        holdFor(3.0f, [this]
        {
            fadeOut(1.0f, ml::EXPONENTIAL);
        });
    }

    void setPosition(const sf::Vector2f& pos) override { _text.setPosition(pos); }
    sf::Vector2f  getPosition()     const override { return _text.getPosition(); }
    sf::FloatRect getGlobalBounds() const override { return _text.getGlobalBounds(); }

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
    {
        sf::Color c = _text.getFillColor();
        c.a = getAlpha();
        _text.setFillColor(c);
        target.draw(_text, states);
    }

    bool isDone() const { return !isFading() && getAlpha() == 0; }
};
