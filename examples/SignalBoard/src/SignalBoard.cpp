// ============================================================
//  SignalBoard.cpp
// ============================================================
//
//  LAYOUT STRATEGY
//  ───────────────
//  Rather than calculating every pixel manually, we pick one
//  anchor and let Malena's Positionable helpers chain everything
//  else into place:
//
//    1. EventLog   → absolute anchor (right side of window)
//    2. Tower D    → setLeftOf(log)
//    3. Tower C    → setLeftOf(towerD)
//    2. Tower B    → setLeftOf(towerC)
//    1. Tower A    → setLeftOf(towerB)
//    5. Buttons    → setBelow(each tower), then setRightOf chain
//    6. Fire All   → setBelow(btnA)
//    7. Reset      → setRightOf(fireAll)
//    8. Score      → Align::setAbove + Align::centerHorizontally
//                    over the combined tower bounds
//
// ============================================================

#include "SignalBoard.h"
#include <Malena/Utilities/Align.h>

// ── Spacing constants ─────────────────────────────────────────────────────────
namespace Spacing
{
    constexpr float TOWER_W   = 80.f;
    constexpr float TOWER_H   = 120.f;
    constexpr float TOWER_GAP = 20.f;   // gap between towers and buttons

    constexpr float BTN_H     = 40.f;
    constexpr float BTN_ROW   = 28.f;   // gap: tower bottom → button top
    constexpr float WIDE_ROW  = 12.f;   // gap: button row  → fire/reset row

    constexpr float LOG_W     = 180.f;
    constexpr float LOG_H     = 320.f;
    constexpr float LOG_RIGHT = 20.f;   // gap from window right edge
    constexpr float LOG_TOP   = 50.f;   // absolute Y anchor for the log

    constexpr float WIN_W     = 800.f;
    constexpr float WIN_H     = 520.f;

    constexpr float SCORE_GAP = 18.f;   // gap above the tower group
}

// ── Tower colour palette ──────────────────────────────────────────────────────
struct TowerDef { const char* name; sf::Color color; };
static constexpr TowerDef DEFS[4] = {
    { "Tower A", { 100, 180, 255, 255 } },
    { "Tower B", { 255, 160,  80, 255 } },
    { "Tower C", { 120, 220, 120, 255 } },
    { "Tower D", { 220, 100, 200, 255 } },
};

// ─────────────────────────────────────────────────────────────────────────────

SignalBoard::SignalBoard()
    : Application(
          static_cast<unsigned int>(Spacing::WIN_W),
          static_cast<unsigned int>(Spacing::WIN_H),
          32,
          "SignalBoard - Malena Events and Messaging Demo")
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  initialization()
// ─────────────────────────────────────────────────────────────────────────────

void SignalBoard::initialization()
{
    // ── EventLog — the right-side layout anchor ───────────────────────────────
    //  One absolute setPosition call anchors the whole right column.
    //  Everything on the left is positioned relative to this.
    _log = std::make_unique<EventLog>(
        sf::Vector2f{ Spacing::WIN_W - Spacing::LOG_W - Spacing::LOG_RIGHT,
                      Spacing::LOG_TOP },
        sf::Vector2f{ Spacing::LOG_W, Spacing::LOG_H }
    );
    addComponent(*_log);

    // ── ScoreDisplay — constructed first so towers can go below it ────────────
    _score = std::make_unique<ScoreDisplay>(sf::Vector2f{ 0.f, 0.f });
    addComponent(*_score);

    // ── Towers — chain leftward from EventLog ─────────────────────────────────
    //  Tower D sits directly to the left of the log.
    //  Each preceding tower sits to the left of the next one.
    //  All towers share the same Y because setLeftOf preserves Y.
    for (int i = 0; i < 4; ++i)
    {
        _towers.push_back(std::make_unique<SignalTower>(
            i, DEFS[i].name, DEFS[i].color,
            sf::Vector2f{ 0.f, Spacing::LOG_TOP }   // x is overridden below
        ));
        addComponent(*_towers.back());
    }

    // D is leftmost relative to log, then chain C←D, B←C, A←B
    _towers[3]->setLeftOf(*_log,       Spacing::TOWER_GAP);
    _towers[2]->setLeftOf(*_towers[3], Spacing::TOWER_GAP);
    _towers[1]->setLeftOf(*_towers[2], Spacing::TOWER_GAP);
    _towers[0]->setLeftOf(*_towers[1], Spacing::TOWER_GAP);

    // ── ScoreDisplay — above and centred over the tower group ─────────────────
    //  Build a FloatRect that spans all four towers so we can use the
    //  Align static helpers to place and centre the score in one go.
    {
        auto  t0 = _towers[0]->getGlobalBounds();
        auto  t3 = _towers[3]->getGlobalBounds();
        sf::FloatRect allTowers{
            t0.position,
            { t3.position.x + t3.size.x - t0.position.x, t0.size.y }
        };

        // setAbove: positions score directly above the tower group
        ml::Align::setAbove(allTowers, *_score, Spacing::SCORE_GAP);

        // centerHorizontally: slides score to the horizontal midpoint
        // of the tower group, keeping the Y we just set
        ml::Align::centerHorizontally(allTowers, *_score);
    }

    ml::RectangleButton* btns[4] = { &_btnA, &_btnB, &_btnC, &_btnD };

    for (int i = 0; i < 4; ++i)
    {
        btns[i]->setSize({ Spacing::TOWER_W, Spacing::BTN_H });
        btns[i]->setFillColor(sf::Color(30, 30, 50));
        btns[i]->setOutlineThickness(1.5f);
        btns[i]->setOutlineColor(DEFS[i].color);
        btns[i]->setString(std::string(1, char('A' + i)));
        addComponent(*btns[i]);
    }

    // Button A directly below tower A
    _btnA.setBelow(*_towers[0], Spacing::BTN_ROW);

    // Buttons B-D chain rightward from A, keeping the same gap as the towers
    _btnB.setRightOf(_btnA, Spacing::TOWER_GAP);
    _btnC.setRightOf(_btnB, Spacing::TOWER_GAP);
    _btnD.setRightOf(_btnC, Spacing::TOWER_GAP);

    // ── Broadcast buttons — setBelow the button row ───────────────────────────
    //  "Fire All" goes below button A (same X, preserving the left edge).
    //  "Reset" sits to the right of "Fire All".
    float wideW = (Spacing::TOWER_W * 2 + Spacing::TOWER_GAP);

    _btnFireAll.setSize({ wideW, Spacing::BTN_H });
    _btnFireAll.setFillColor(sf::Color(30, 30, 50));
    _btnFireAll.setOutlineThickness(1.5f);
    _btnFireAll.setOutlineColor(sf::Color(200, 165, 60));
    _btnFireAll.setString("Fire All");
    addComponent(_btnFireAll);
    _btnFireAll.setBelow(_btnA, Spacing::WIDE_ROW);

    _btnReset.setSize({ wideW, Spacing::BTN_H });
    _btnReset.setFillColor(sf::Color(30, 30, 50));
    _btnReset.setOutlineThickness(1.5f);
    _btnReset.setOutlineColor(sf::Color(200, 80, 80));
    _btnReset.setString("Reset");
    addComponent(_btnReset);
    _btnReset.setRightOf(_btnFireAll, Spacing::TOWER_GAP);
}

// ─────────────────────────────────────────────────────────────────────────────
//  registerEvents()
// ─────────────────────────────────────────────────────────────────────────────

void SignalBoard::registerEvents()
{
    // Individual tower buttons — publish targeted string event
    // _btnA.onClick([]() { ml::EventManager::fire(Events::towerFire(0)); });
    // _btnB.onClick([]() { ml::Subscribable::publish(Events::towerFire(1)); });
    // _btnC.onClick([]() { ml::Subscribable::publish(Events::towerFire(2)); });
    // _btnD.onClick([]() { ml::Subscribable::publish(Events::towerFire(3)); });
    //
    // // Broadcast buttons
    // _btnFireAll.onClick([]() { ml::Subscribable::publish(Events::FIRE_ALL);  });
    // _btnReset.onClick  ([]() { ml::Subscribable::publish(Events::RESET_ALL); });

    // Hover feedback — highlight each button in its tower's colour on hover
    auto addHover = [](ml::RectangleButton& btn, sf::Color accent)
    {
        btn.onHover  ([&btn, accent]() { btn.setFillColor(accent); });
        btn.onUnhover([&btn]()         { btn.setFillColor(sf::Color(30, 30, 50)); });
    };

    addHover(_btnA,       DEFS[0].color);
    addHover(_btnB,       DEFS[1].color);
    addHover(_btnC,       DEFS[2].color);
    addHover(_btnD,       DEFS[3].color);
    addHover(_btnFireAll, sf::Color(200, 165, 60));
    addHover(_btnReset,   sf::Color(200,  80, 80));
}
