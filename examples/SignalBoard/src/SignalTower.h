#pragma once
// ============================================================
//  SignalTower.h
// ============================================================
//
//  KEY CONCEPTS:
//    ml::CoreAdapter   — concrete base implementing the three
//                        Positionable pure virtuals.
//    ml::Messenger     — opt-in trait for sendMessage/onMessage.
//    AssetsManager     — fetches the font from the manifest cache.
//                        Called once in the constructor; the cached
//                        reference is stored so draw() never triggers
//                        a file load.
//    subscribe()       — register for named string events
//    sendMessage<T>()  — push typed data when the tower activates
//
// ============================================================

#include <Malena/Core/CoreAdapter.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Resources/AssetsManager.h>
#include <SFML/Graphics.hpp>
#include "SignalBoardManifest.h"
#include "Events.h"
#include "Messages.h"

class SignalTower : public ml::Component<ml::Messenger>
{
public:
    SignalTower(int id, const std::string& name,
                sf::Color color, sf::Vector2f position)
        : _id(id), _name(name), _color(color),
          // ── AssetsManager — fetch font once, store the cached reference ──────
          //  The manifest registered "assets/fonts/Outfit-Regular.ttf" at
          //  startup.  get() loads the file on this first call and caches it.
          //  Every subsequent get() — including from other components — returns
          //  the same sf::Font& with no file I/O.
          _font(ml::AssetsManager<SignalBoardManifest>::get(
                    SignalBoardManifest::Fonts::Main))
    {
        _body.setSize({ BODY_W, BODY_H });
        _body.setOutlineThickness(2.f);
        _halo.setSize({ BODY_W + HALO_PAD * 2.f, BODY_H + HALO_PAD * 2.f });

        setPosition(position);
        updateVisuals();

        // ── String-event subscriptions ────────────────────────────────────────
        subscribe(Events::FIRE_ALL,       [this]() { activate(); });
        subscribe(Events::towerFire(_id), [this]() { activate(); });
        subscribe(Events::RESET_ALL,      [this]() { deactivate(); });

        onUpdate([this]()
        {
            if (_glowTimer > 0.f)
            {
                _glowTimer -= 1.f / 60.f;
                if (_glowTimer <= 0.f) deactivate();
                else updateVisuals();
            }
        });
    }

    // ── Positionable ──────────────────────────────────────────────────────────
    void setPosition(const sf::Vector2f& pos) override
    {
        _body.setPosition(pos);
        _halo.setPosition({ pos.x - HALO_PAD, pos.y - HALO_PAD });
    }
    sf::Vector2f  getPosition()     const override { return _body.getPosition(); }
    sf::FloatRect getGlobalBounds() const override { return _body.getGlobalBounds(); }

    // ── Draw ──────────────────────────────────────────────────────────────────
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        if (_active || _glowTimer > 0.f)
            target.draw(_halo, states);
        target.draw(_body, states);

        // _font is the cached reference stored in the constructor — no file I/O here
        sf::Text label(_font, _name.substr(_name.size() - 1), 15);
        label.setFillColor(sf::Color::White);
        auto lb = label.getLocalBounds();
        auto bb = _body.getGlobalBounds();
        label.setPosition({
            bb.position.x + (bb.size.x - lb.size.x) / 2.f - lb.position.x,
            bb.position.y + (bb.size.y - lb.size.y) / 2.f - lb.position.y
        });
        target.draw(label, states);
    }

private:
    void activate()
    {
        _active    = true;
        _glowTimer = GLOW_DURATION;
        updateVisuals();

        // ── Typed message (Messenger trait) ───────────────────────────────────
        sendMessage<TowerFiredData>(GameMsg::TowerFired,
            TowerFiredData{ _id, _color, _name });
    }

    void deactivate()
    {
        _active    = false;
        _glowTimer = 0.f;
        updateVisuals();
    }

    void updateVisuals()
    {
        float alpha = _active ? 1.f
                     : (_glowTimer > 0.f ? _glowTimer / GLOW_DURATION : 0.f);

        sf::Color body = _color;
        body.a = static_cast<uint8_t>(50 + 205.f * alpha);
        _body.setFillColor(body);
        _body.setOutlineColor(_active ? _color : sf::Color(70, 70, 100));

        sf::Color halo = _color;
        halo.a = static_cast<uint8_t>(90.f * alpha);
        _halo.setFillColor(halo);
    }

    int                _id;
    std::string        _name;
    sf::Color          _color;
    bool               _active    = false;
    float              _glowTimer = 0.f;
    const sf::Font&    _font;       // cached reference from AssetsManager

    sf::RectangleShape _body;
    sf::RectangleShape _halo;

    static constexpr float BODY_W        = 80.f;
    static constexpr float BODY_H        = 120.f;
    static constexpr float HALO_PAD      = 10.f;
    static constexpr float GLOW_DURATION = 1.5f;
};
