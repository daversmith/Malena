#pragma once
// ============================================================
//  Messages.h  —  typed message definitions for SignalBoard
// ============================================================
//
//  Malena's Messenger trait delivers type-safe, enum-keyed data
//  between objects.  Unlike string events (which are fire-and-
//  forget with no payload), typed messages carry a struct and
//  are checked by the compiler.
//
//  IMPORTANT: Messenger is NOT part of ml::Component by default.
//  A class must opt in explicitly:
//
//    class MyComp : public ml::Component<void, ml::Messenger> { ... };
//
//  Then:
//    // Send (in a Messenger subclass):
//    sendMessage<TowerFiredData>(GameMsg::TowerFired, { id, color, name });
//
//    // Receive (in a Messenger subclass):
//    onMessage<TowerFiredData>(GameMsg::TowerFired,
//        [this](const TowerFiredData& d) { ... });
//
// ============================================================

#include <SFML/Graphics/Color.hpp>
#include <string>

// ── Message enum ──────────────────────────────────────────────────────────────
//
//  A single scoped enum covers every typed message in this program.
//  Scoped ("class") enum prevents accidental name collisions.
//
enum class GameMsg
{
    TowerFired,  ///< A tower was activated.  Payload: TowerFiredData
    ScoreUpdate, ///< The cumulative signal count changed.  Payload: int
};

// ── Payload structs ───────────────────────────────────────────────────────────

/// Data attached to a GameMsg::TowerFired message.
/// Pure data — no methods, no logic.
struct TowerFiredData
{
    int         id;    ///< Which tower was activated (0–3)
    sf::Color   color; ///< The tower's display colour
    std::string name;  ///< Human-readable label, e.g. "Tower A"
};
