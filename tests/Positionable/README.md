# Positionable

Locks the **spatial contract** every `Core` exposes — position, global bounds,
relative-layout helpers, and the animated-movement queue. Uses `ml::Rectangle`,
a concrete shape that implements `setPosition` / `getPosition` /
`getGlobalBounds`.

## What's tested

- `test_position_and_bounds` — `getPosition()` echoes what was set, and
  `getGlobalBounds()` reflects both position and size (and follows the component
  when it moves). Hit-testing and layout depend on bounds tracking position.
- `test_relative_layout` — the deterministic geometry helpers:
  `setRightOf(obj, gap)`, `setBelow(obj, gap)`, and `center(obj)` compute the
  expected pixel coordinates.
- `test_movement_queue` — `isScrolling()` is `false` at rest and becomes `true`
  after `moveTo(target, seconds)` populates the waypoint queue.

## How it works

Window-free. Note the **timed** part of animation (advancing the queue over real
seconds via `onUpdate`) is clock-dependent and **not** asserted here — only the
queue's *populated/empty* state is, which is deterministic. Bounds use SFML 3's
`FloatRect` (`.position` / `.size`). See [`../README.md`](../README.md) for
shared mechanics.

## Pass / fail

Pass prints `Positionable: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. Comparisons use a small epsilon
(`near`, 0.5px) to tolerate float rounding; a fail means position/bounds or a
layout helper computes the wrong coordinate.
