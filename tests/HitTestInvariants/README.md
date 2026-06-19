# HitTestInvariants

Locks the three **gates the click filter composes** — the exact properties whose
violation produced the "toolbar needs two clicks" class of bugs (origin-parked,
hidden-ancestor, or not-under-owner components silently staying click-testable).

The dispatcher's filter accepts a click target only if it is **enabled**,
**effectively visible**, **under the active exclusive owner**, and hovered.
`isHovered()` is pure SFML geometry and left to manual/UI testing; this suite
pins the other three gates.

## What's tested

- **Gate 1 — `test_enabled_flag_matches_state`**: a `Core`'s `isEnabled()` and
  its `Flag::ENABLED` must never disagree. If they diverge, a control can look
  disabled yet still pass the filter (the original `Select::setEnabled` bug).
- **Gate 2 — `test_effective_visibility_through_ancestor` / `..._two_levels`**: a
  component inside a hidden container is not drawn and must report
  `isEffectivelyVisible() == false`, even though its *own* `HIDDEN` flag still
  says visible. Verified one and two ancestors up.
- **Gate 3 — `test_exclusive_owner_gating`**: with an owner set (a Modal / open
  Select), only the owner and its descendants are interactive; unrelated
  components are locked out. Also checks behavior after reparenting.

## How it works

Window-free; drives `Core` / `AppManager` logic directly — no window, fonts, or
SFML resources. See [`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `HitTestInvariants: all checks passed` and exits `0`. A failure
prints the failing gate with `file:line` and exits `1` — a fail here means a
component that should be unclickable can still receive clicks (or vice versa),
i.e. the click-routing bug class has regressed.
