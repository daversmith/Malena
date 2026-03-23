#pragma once
// ============================================================
//  Events.h  —  SignalBoard named event constants
// ============================================================
//
//  Malena's string-based pub/sub system lets any Subscribable
//  object fire or listen to a named event.  Storing the names
//  here as constexpr strings:
//    • prevents typos (compile-time errors vs. silent runtime bugs)
//    • makes every event in the plugin searchable in one place
//    • lets both the sender and receiver share the exact same string
//
//  PATTERN:
//    // Subscribe (in any Subscribable subclass):
//    subscribe(Events::FIRE_ALL, [this]() { ... });
//
//    // Publish (static — callable from anywhere):
//    ml::Subscribable::publish(Events::FIRE_ALL);
//
// ============================================================

#include <string>

namespace Events
{
    // ── Broadcast events (one sender → all towers) ───────────────────────────

    /// Activate every tower simultaneously.
    inline constexpr const char* FIRE_ALL  = "tower.fire_all";

    /// Dim every tower and clear the board.
    inline constexpr const char* RESET_ALL = "tower.reset_all";

    // ── Per-tower targeted events ────────────────────────────────────────────
    //
    //  Returns a unique event string for a specific tower index.
    //  Tower 0 subscribes to "tower.fire.0",
    //  Tower 1 subscribes to "tower.fire.1", and so on.
    //
    //  Both the tower (subscriber) and the button (publisher) call this
    //  same helper, so the strings are always in sync.
    //
    inline std::string towerFire(int id)
    {
        return "tower.fire." + std::to_string(id);
    }
}
