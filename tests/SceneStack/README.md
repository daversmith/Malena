# SceneStack

Locks the **owning stack of layered scenes**: `ml::SceneStack` keeps scenes
bottom to top, enables only the top one (and, through the enable cascade, only
its controls), hides scenes covered by an opaque scene, keeps a scene drawn
under an overlay, and destroys scenes when they are popped.

## What's tested

Using a `Scene : Component<>, SceneLifecycle` that owns one child control and
logs its lifecycle hooks and its own destruction:

- `test_push_outside_dispatch_applies_now` — a push made outside event dispatch
  takes effect immediately; the new top is enabled and visible.
- `test_opaque_cover_and_pop` — an opaque push disables and hides the scene
  underneath **and its child control**; `pop()` restores both, fires
  `onSceneResume`, and deletes the popped scene.
- `test_overlay_keeps_below_drawn` — `pushOverlay` leaves the scene underneath
  visible but disabled; scenes below an opaque scene stay hidden.
- `test_lifecycle_order_and_replace` — hooks fire as enter → pause → exit/delete
  → resume, and `replace()` swaps the top without resuming and re-pausing the
  scene underneath.
- `test_pop_inside_dispatch_is_deferred` — a scene that pops itself from its own
  update handler is still alive when the handler continues; the pop applies on
  the stack's next update.
- `test_saved_changes_keep_order` — changes saved during dispatch apply in request
  order, so push-then-pop removes the new scene, not the one that asked.
- `test_covered_scene_stops_updating` — a covered scene's `onUpdate` stops (this
  relies on `onUpdate` skipping disabled components) and resumes when uncovered.
- `test_clear_then_push` — `clear()` removes everything requested before it; a
  push requested after it survives.
- `test_of_finds_owner` — `SceneStack::of(scene)` finds the stack for live
  scenes and for scenes whose push is still waiting; `nullptr` otherwise.
- `test_edges_and_teardown` — popping past empty is harmless, and the stack's
  destructor deletes the scenes it still holds.

## How it works

Window-free. Event dispatch is simulated with `ml::UpdatableDispatcher::fire()`,
which marks the `EventManager` busy — the condition under which the stack saves
changes instead of applying them. Liveness is observed through `Flag::ENABLED`
and `isVisible()`; destruction through the scene's destructor log. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `SceneStack: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A crash (exit 139) in the
deferred-pop test means a scene was destroyed while its own handler was running.
