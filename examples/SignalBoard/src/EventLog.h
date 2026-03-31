#pragma once
// ============================================================
//  EventLog.h  —  scrolling log driven by typed messages
// ============================================================
//
//  KEY CONCEPTS:
//    onMessage<T>()    — receive typed enum message with payload
//    sendMessage<T>()  — chain a new message downstream
//    AssetsManager     — font fetched once in the constructor
//                        and stored as a const ref member
//
// ============================================================

#include <../../../include/Malena/Traits/Communication/Messenger.h>
#include <Malena/Core/CoreAdapter.h>
#include <Malena/Resources/AssetsManager.h>
#include <SFML/Graphics.hpp>
#include <deque>
#include "Messages.h"
#include "SignalBoardManifest.h"

class EventLog : public ml::Component<ml::Messenger>
{
public:
    EventLog(sf::Vector2f position, sf::Vector2f size)
        : _size(size),
          // ── Font from manifest cache — loaded once, stored as ref ─────────
          _font(ml::AssetsManager<SignalBoardManifest>::get(
                    SignalBoardManifest::Fonts::Main)),
          _header(_font, "Event Log", 13)
    {
        _bg.setSize(size);
        _bg.setFillColor(sf::Color(16, 16, 28));
        _bg.setOutlineThickness(1.f);
        _bg.setOutlineColor(sf::Color(70, 70, 110));
        _header.setFillColor(sf::Color(150, 130, 210));

        setPosition(position);

        // ── Typed message subscription ────────────────────────────────────────
        //  onMessage<PayloadType>(EnumValue, callback)
        //  Template arg must match the sendMessage call in SignalTower.
        onMessage<TowerFiredData>(GameMsg::TowerFired,
            [this](const TowerFiredData& data)
            {
                ++_total;
                _entries.push_front(Entry{
                    "[" + data.name + "] fired  (total: "
                        + std::to_string(_total) + ")",
                    data.color, 255
                });
                if (_entries.size() > MAX_ENTRIES)
                    _entries.pop_back();

                // Chain: EventLog is also a sender
                sendMessage<int>(GameMsg::ScoreUpdate, _total);
            });

        onUpdate([this]()
        {
            for (auto& e : _entries)
            {
                if (e.age < HOLD_FRAMES) { ++e.age; continue; }
                if (e.alpha > 4) e.alpha -= 4;
            }
            while (!_entries.empty() && _entries.back().alpha <= 4)
                _entries.pop_back();
        });
    }

    // ── Positionable ──────────────────────────────────────────────────────────
    void setPosition(const sf::Vector2f& pos) override
    {
        _pos = pos;
        _bg.setPosition(pos);
        _header.setPosition(sf::Vector2f{ pos.x + 10.f, pos.y + 8.f });
    }
    sf::Vector2f  getPosition()     const override { return _pos; }
    sf::FloatRect getGlobalBounds() const override { return _bg.getGlobalBounds(); }

    // ── Draw ──────────────────────────────────────────────────────────────────
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(_bg,     states);
        target.draw(_header, states);

        float y = _pos.y + 32.f;
        for (const auto& e : _entries)
        {
            sf::Text line(_font, e.text, 12);  // _font = cached ref, no file I/O
            sf::Color c = e.color;
            c.a = e.alpha;
            line.setFillColor(c);
            line.setPosition(sf::Vector2f{ _pos.x + 10.f, y });
            target.draw(line, states);
            y += 20.f;
        }
    }

private:
    struct Entry
    {
        std::string text;
        sf::Color   color;
        uint8_t     alpha = 255;
        int         age   = 0;
    };

    sf::Vector2f       _size;
    sf::Vector2f       _pos;
    const sf::Font&    _font;   // cached reference from AssetsManager
    sf::RectangleShape _bg;
    sf::Text           _header;
    std::deque<Entry>  _entries;
    int                _total = 0;

    static constexpr std::size_t MAX_ENTRIES = 8;
    static constexpr int         HOLD_FRAMES = 200;
};
