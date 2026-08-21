# Framework Plan — Retained Anchors + Animate rewrite

Two related framework changes to Malena:

- **Part A — Retained anchors:** make the existing `Align`/`Positionable` relational
  placement *sticky* so layouts re-solve on resize (responsive by default), instead of
  being one-shot pixel math re-run by hand in every `fill()`.
- **Part B — Animate:** extract position animation out of `Positionable` into a new,
  stable, delta-time `Animate` subsystem with a real easing library. The current
  animation code is unstable and unused, so this is a clean rewrite.

Both reuse the vocabulary that already exists — **no new positioning verbs to learn.**

---

## 0. What exists today (and what's wrong)

### 0.1 Relational placement — solid, but one-shot
`ml::Align` (static) + `Positionable` (instance) already provide the full vocabulary:

| Verb | Reference kinds |
|------|-----------------|
| `setRightOf` / `setLeftOf` / `setBelow` / `setAbove` | component `T`, `sf::FloatRect`, `sf::Vector2` (size) |
| `centerOn` / `centerHorizontally` / `centerVertically` | same three |
| `centerText` | text-in-object |

`Positionable::setBelow(...)` just forwards to `Align::setBelow(...)`, which computes a
position **once** and calls `setPosition`. Nothing is remembered, so a resize doesn't
re-run it — which is exactly why every `fill()` recomputes all positions by hand.
Used widely already (AppShell, Settings panels, StudentShell, ConsentScene…).

### 0.2 Animation — baked into `Positionable`, broken, and unused
`Positionable` privately carries `velocityX/Y`, `framerate`, `clock`, `points` queue,
`scrolling`, `initialPosition`, plus `moveTo` / `moveDistance` / `isScrolling` /
`generatePoints` / `generateExponential` / `calcVelocity` / `error`. Problems:

1. **Frame-rate dependent, no delta time.** `calcVelocity` bakes a per-frame step of
   `(target-current)/seconds/framerate` assuming exactly `framerate` fps and adds a
   fixed velocity each call. Real fps ≠ 60 → wrong speed. `clock` is declared but unused.
2. **Debug spam.** `generateExponential` prints to `std::cout` every call.
3. **Two half-wired implementations.** `moveTo` uses the velocity path; `generatePoints`
   populates a `points` queue that is **never consumed**. Dead code.
4. **The `Tween` param isn't even wired.** Docs show `moveTo(pos, sec, EXPONENTIAL)`, but
   the real signatures are `(Vector2f,float)` / `(FloatRect,float)` — no easing arg.
   The enum only meaningfully handles `EXPONENTIAL`; `LINEAR` falls through a switch.
5. **Inconsistent sentinels.** Header inits `initialPosition = {FLT_MIN, FLT_MAX}`; the
   `.cpp` compares against `{FLT_MAX, FLT_MAX}`. Latent bug in `moveDistance`.
6. **Magic constants.** `while (count < 720)` (screen height?) in the exponential path.
7. **Zero callers.** `grep` across Malena (src/examples/tests) and LockIn finds **no**
   uses of `moveTo`/`moveDistance`/`isScrolling`. Removing them breaks nothing.

Because of #7, Part B is a free rewrite: delete the animation state from `Positionable`
and rebuild it correctly in a dedicated subsystem.

---

## Part A — Retained anchors

### A.1 Semantics (agreed)
- **Retained by default (opt-out).** Calling a relational verb records the relationship
  AND applies it immediately; the framework re-applies it whenever the reference (or
  window) changes size/position.
- **`setPosition()` detaches.** An absolute position is manual control and wins; it clears
  the component's stored anchors. This makes the "anchor vs manual move" fight impossible.
- **Live references only.** Retention tracks a *component* or *the window*. The raw
  `sf::FloatRect` / `sf::Vector2` overloads stay **one-shot snapshots** (there's nothing
  live to re-read). Component/window anchors are the retained ones.
- **Explicit opt-out:** `unanchored()` freezes current positions (drops anchors without
  moving anything); `reanchor()`/re-calling a verb re-attaches.

### A.2 Data model
Add to `Positionable` (lazily — empty unless used):

```cpp
enum class AnchorOp { LeftOf, RightOf, Above, Below, CenterOn, CenterX, CenterY };

struct Anchor {
    AnchorOp        op;
    Positionable*   ref;      // live reference; nullptr => the window
    float           spacing;  // gap / offset
};

std::vector<Anchor> _anchors;   // applied in registration order; last-wins per axis
bool                _anchorsEnabled = true;
```

- A component can hold multiple anchors (e.g. `centerX(parent)` + `below(header,12)`);
  they're applied in order, so X and Y intents compose. Re-calling the *same* op with the
  *same* ref updates spacing in place rather than duplicating.
- `ref == nullptr` is the **window sentinel** — resolved against the current window rect
  at solve time. New no-arg / window overloads: `centerHorizontally()` (in window),
  `anchorToWindow(...)`, or reuse existing verbs with a `WindowRef` tag. (Decision A-Q1.)

### A.3 API changes (`Positionable`)
- Each relational verb (`setBelow`, `center*`, …): if `_anchorsEnabled`, push/update an
  `Anchor`, then call `solveAnchors()` for immediate placement.
- `setPosition(v)`: if not called *by* the solver, `_anchors.clear()` (manual override).
  Guard with an internal `_solving` flag so the solver's own `setPosition` doesn't detach.
- `unanchored()`: `_anchorsEnabled = false; _anchors.clear();`
- `solveAnchors()`: for each anchor, resolve the reference rect (component bounds, or
  window rect if `ref==nullptr`) and delegate to the existing `Align::` op with `_solving=true`.

### A.4 Who re-solves, and when
Anchors reference parents/siblings/window — a tree relationship — so drive solving from
the container, cascading through the unified `_children` tree:

- `Core`/`Panel` gains `relayoutAnchoredChildren()`: walk `_children` in registration
  order, call each child's `solveAnchors()`, then recurse. Registration order means a
  child anchored to an earlier sibling resolves after it in a **single pass** (the common
  case). Do at most **2 passes** to settle forward references; detect no-change to stop.
- Triggers:
  - **Window resize** → root re-solve (piggyback the size-poll I already added to
    `AppShell`, promoted to a framework-level `onWindowResize` → root
    `relayoutAnchoredChildren`).
  - **A container's own `setPosition`/`setSize`** → re-solve its subtree (so moving a
    panel drags its anchored children, and resizing it re-centers them).
- **Cycle guard:** if two passes don't settle (A↔B), stop and (debug build) warn once with
  the component types. No infinite loop.
- **Lifetime:** a stored `ref` is a sibling/parent in the same tree; when a child is
  unlinked (`_children` removal / `Panel::clear`), also scrub it from any sibling's
  `_anchors`. (Anchors to out-of-tree components are discouraged; document it.)

### A.5 Adoption (LockIn, later phase)
- Convert `AppShell`'s manual reflow (`_content.setBelow(_toolbar)` + hand-sizing) to
  retained anchors; drop the explicit re-fill-on-resize where anchors cover it.
- Convert a scene that currently draws at fixed coords — **student `TitleScene`** — as the
  proving ground (logo `centerX(window)` + `centerY(window,-40)`, subtitle `below(logo)`).
- Leave other screens on `fill()` until touched (retained verbs are backward-compatible:
  existing one-shot `setBelow` in a `fill()` that already re-runs is just harmlessly sticky).

### A.6 Open decisions
- **A-Q1:** window reference spelling — no-arg overloads (`centerHorizontally()` = window),
  a `WindowRef` tag type, or an explicit `anchorToWindow*`. Leaning: no-arg overloads for
  the common centers + a `ml::Anchor::window()` reference object for edges.
- **A-Q2:** should a container *auto*-anchor children added via `addComponent` with a
  layout arg, or is anchoring always an explicit per-child call? Leaning explicit.

---

## Part B — Animate (extract + rewrite)

### B.1 Goals
- Remove all animation state/methods from `Positionable` (dead + broken).
- New subsystem: **stable, delta-time, reusable** for any interpolatable property
  (position, color, scale, opacity, scroll), with a proper easing library.
- Common case stays one call; the engine underneath is generic.

### B.2 Pieces
1. **`ml::Easing`** — a library of pure easing functions `float ease(float t)` on `t∈[0,1]`:
   `Linear`, `EaseIn/Out/InOut` for `Quad/Cubic/Quart/Quint/Sine/Expo/Circ`, plus
   `Back`, `Elastic`, `Bounce`. Correct, unit-tested, no state. Replaces the `Tween` enum
   (kept as a thin deprecated alias: `LINEAR`→`Easing::Linear`, `EXPONENTIAL`→`Easing::EaseOutExpo`).
2. **`ml::Tween<T>`** — generic value animator. Interpolates any lerpable `T`
   (`float`, `sf::Vector2f`, `sf::Color`) from `start`→`end` over `duration` using an
   easing fn, advanced by **real dt**. Features: `delay`, `onUpdate(value)` /
   `onComplete()` callbacks, `reverse`, `loop`/`pingPong`, `cancel()`, `bool done()`.
   `lerp(T,T,float)` specialized per type (component-wise for Vector2f/Color).
3. **`Animate` driver on `Core`** — owns active tweens keyed by target property; advanced
   each frame from the existing update tick with `dt`; applies results via the right setter
   (`setPosition`, `setFillColor`, `setScale`…). Lazily allocated (no cost until used).

### B.3 Delta time
Source real elapsed time, never an assumed framerate:
- `AppManager::run` already has the frame loop; pass `dt` (seconds) into the update
  dispatch so `Animate::advance(dt)` integrates correctly. (Or `Animate` holds its own
  `sf::Clock`; prefer a single authoritative dt from the loop to keep everything in sync.)

### B.4 API sketch
```cpp
// common case — one line:
btn.animate().move({400,300}, 0.4f, Easing::EaseOutCubic).onComplete([]{ /* … */ });

// other properties, same engine:
panel.animate().fade(0.f, 0.25f);                 // opacity → 0
card.animate().scale({1.1f,1.1f}, 0.15f, Easing::EaseOutBack);
label.animate().color(sf::Color::Red, 0.3f);

// control:
btn.animate().cancel();          bool busy = btn.animate().active();
```
- Convenience shims on `Positionable`/`Core` (`animateTo(pos,dur,ease)`) for the 90% case.

### B.5 Interaction with anchors
Consistent with Part A's rule: an animation calls `setPosition` each frame, which
**detaches the anchor** (manual/animated control wins). So "animate a component that was
anchored" just works and leaves it detached at the end — re-call a verb to re-anchor.
(If we later want animated-and-re-anchored, the anchor can be re-applied in `onComplete`.)

### B.6 Removal / compat
- Delete from `Positionable`: `velocityX/Y`, `framerate`, `clock`, `points`, `scrolling`,
  `initialPosition`, `moveTo(x2)`, `moveDistance`, `isScrolling`, `setFramerate`,
  `generatePoints`, `generateExponential`, `calcVelocity`, `error`. (Zero callers.)
- Keep `Tween` enum as a deprecated alias into `Easing` for source compatibility; new code
  uses `Easing`.

---

## Phasing

1. **Phase 1 — Animate foundation** (independent, no anchor dependency)
   `Easing` + `Tween<T>` + `Animate` driver + dt from the loop; delete dead
   `Positionable` animation code; deterministic unit tests. Ship first — self-contained,
   removes known-broken code.
2. **Phase 2 — Retained anchors**
   `Anchor` storage on `Positionable`, retained verbs, detach-on-`setPosition`,
   container/window solve hook, 2-pass + cycle guard, lifetime scrub; unit tests.
3. **Phase 3 — LockIn adoption**
   Convert AppShell reflow + student `TitleScene` to anchors; verify responsive in a live
   window (monitor board + student title reflow). One scene as proving ground first.
4. **Phase 4 — polish**
   Window-anchor spelling (A-Q1), richer easings, animation `loop`/`pingPong`, docs +
   examples (an `AnchorDemo` / `AnimateDemo` under `examples/`).

## Testing
- **Easing/Tween:** deterministic — feed fixed dt steps, assert value at t=0/0.5/1, assert
  `done()` timing, callbacks fire once, pingpong/loop.
- **Anchors:** headless — construct parent+children, resize parent, assert child rects;
  detach-on-`setPosition`; cycle guard terminates; lifetime scrub after `clear()`.
- **Runtime smoke:** responsive scenes need a live window (per project history, layout +
  hit-testing can't be verified headlessly) — check TitleScene/monitor board reflow on a
  real drag-resize.

## Risks
- **Anchor solve ordering / cycles** — mitigated by registration-order single pass + 2-pass
  cap + cycle guard.
- **Retained-by-default blast radius** — every existing one-shot `setBelow`/`center*`
  becomes sticky. Mostly redundant-safe (they live in `fill()`s that re-run anyway), and
  detach-on-`setPosition` contains fights. Watch for code that positions relationally then
  expects to hand-move without calling `setPosition` (rare).
- **`Animate` state on `Core`** — keep lazily allocated so non-animated components pay nothing.
```
