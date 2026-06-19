# ControlCascade

Locks **per-control enable/disable behavior** and **container cascades** — the
keystone fixes behind the "toolbar two-clicks" bug. The core rule: a control's
disabled state must be a single source of truth (`ml::Flag::ENABLED`), and
disabling a container must reach everything inside it.

## What's tested

Uses real controls (default font), still window-free:

- `test_select_setEnabled_drives_enabled_flag` — `Select::setEnabled(false)`
  clears `Flag::ENABLED` (the regression: it used to toggle only a private
  `DISABLED` flag, leaving the hit-test filter seeing it as live).
- `test_select_disabled_by_parent_cascade` — a Select disabled by an ancestor
  also reads as not-`ENABLED`.
- `test_all_controls_enabled_flag_invariant` — parametrized over Select /
  PillToggle / SegmentToggle / RadioButton / ButtonToggle / TextArea: each
  control's `isEnabled()` and `Flag::ENABLED` always agree.
- `test_select_open_release_exclusive_owner` — an open Select grabs exclusive
  input and releases it cleanly on close (the actual toolbar-blocking mechanism).
- `test_disabled_select_cannot_open` — `open()` is gated on `Flag::ENABLED`, so a
  disabled Select can't grab ownership even via a direct `open()` call.
- container cascades — disabling **List**, **ScrollPane**, **SplitPanel**,
  **TabbedPanel**, and **Accordion** reaches their children. The TabbedPanel case
  also pins that only the *active* tab's content is live and that switching tabs
  flips enabled-state correctly.

## How it works

Window-free; controls built with `FontManager<>::getDefault()`; exclusive-owner
tests use `AppManager::clearExclusiveOwner()` / `isUnderExclusiveOwner()`. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `ControlCascade: all checks passed` and exits `0`. A failure prints
the offending `CHECK` with `file:line` and exits `1`. A fail here means a
"disabled" control is still click-testable or a container leaks interactivity to
its contents — the bug class this suite exists to prevent.
