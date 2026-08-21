# Flags

Locks the framework's **two state systems** and that they stay independent:

1. **Built-in `ml::Flag`** — the flags framework dispatchers read (`ENABLED`,
   `HOVERED`, `FOCUSED`, `DRAGGABLE`, …), via `FlagManager`'s five-method API.
2. **Custom manifest `Flag` / `State`** — a `ComponentWith<Manifest>` composes
   its own per-manifest `Flag` and `State` enums, separate from `ml::Flag`.

## What's tested

Using a `Widget : ComponentWith<TestManifest>` whose manifest declares
`enum class Flag { Selected, Active }` and `enum class State { Idle, Running, Done }`:

- `test_builtin_flag_api` — `checkFlag` / `enableFlag` / `disableFlag` /
  `toggleFlag` / `setFlag` all behave on `ml::Flag`; `ENABLED` starts true.
- `test_custom_flags_independent` — toggling a custom manifest flag (`Selected`)
  does not disturb a sibling custom flag or the built-in `ml::Flag::ENABLED`.
  This is the structural guarantee behind the flag-unification: a control's own
  flags and the framework's `ENABLED` flag live in separate namespaces.
- `test_state_round_trip` — `setState(...)` then `isState(...)` round-trips.
  (`getState()` with no argument is ambiguous across a component's several state
  managers; `isState(value)` is disambiguated by its argument type and is the
  correct way to read state in a generic context.)

## How it works

Window-free; constructs a `ComponentWith` directly. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `Flags: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A fail means the flag/state
plumbing is broken — e.g. a custom flag bleeding into `ml::Flag::ENABLED`, which
is exactly the inconsistency that caused disabled controls to stay clickable.
