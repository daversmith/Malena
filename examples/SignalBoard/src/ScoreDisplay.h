#pragma once
// ============================================================
//  ScoreDisplay.h  —  score counter driven by typed messages
// ============================================================

#include <../../../include/Malena/Traits/Communication/Messenger.h>
#include <Malena/Core/CoreAdapter.h>
#include <Malena/Resources/AssetsManager.h>
#include <SFML/Graphics.hpp>
#include "Messages.h"
#include "SignalBoardManifest.h"

class ScoreDisplay : public ml::CoreAdapter, public ml::Messenger, public sf::Drawable
{
public:
    explicit ScoreDisplay(sf::Vector2f position)
        : // ── Font from manifest cache — loaded once, stored as ref ─────────
          _font(ml::AssetsManager<SignalBoardManifest>::get(
                    SignalBoardManifest::Fonts::Main))
    {
        setPosition(position);

        onMessage<int>(GameMsg::ScoreUpdate,
            [this](const int& score)
            {
                _score       = score;
                _flashFrames = FLASH_DURATION;
                refresh();
            });

        onUpdate([this]() { if (_flashFrames > 0) --_flashFrames; });
    }

    // ── Positionable ──────────────────────────────────────────────────────────
    void setPosition(const sf::Vector2f& pos) override { _pos = pos; }
    sf::Vector2f  getPosition()     const override { return _pos; }
    sf::FloatRect getGlobalBounds() const override
    {
        // Build a temporary text to measure the rendered bounds so that
        // Align::centerHorizontally can place us accurately.
        const sf::Font& font = ml::AssetsManager<SignalBoardManifest>::get(
                                   SignalBoardManifest::Fonts::Main);
        sf::Text t(font, _displayStr, 22);
        auto lb = t.getLocalBounds();
        return sf::FloatRect(_pos, { lb.size.x, lb.size.y });
    }

    // ── Draw ──────────────────────────────────────────────────────────────────
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        sf::Text t(_font, _displayStr, 22);  // _font = cached ref, no file I/O
        t.setFillColor(_flashFrames > 0 ? sf::Color(255, 220, 80)
                                        : sf::Color(195, 180, 255));
        t.setPosition(_pos);
        target.draw(t, states);
    }

private:
    void refresh() { _displayStr = "Signals fired: " + std::to_string(_score); }

    const sf::Font& _font;        // cached reference from AssetsManager
    sf::Vector2f    _pos;
    std::string     _displayStr  = "Signals fired: 0";
    int             _score       = 0;
    int             _flashFrames = 0;

    static constexpr int FLASH_DURATION = 24;
};
