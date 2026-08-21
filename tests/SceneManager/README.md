# SceneManager

Locks the **instance-based scene router** — the owned `SceneManager<StateEnum>`
that turns an owner's `setState()` calls into scene swaps (add the incoming
scene, remove the outgoing one), with a history stack for `back()` and optional
lazy (construct-on-visit) scenes.

## What's tested

Using an `Owner : ComponentWith<OwnerManifest>` (state enum `{A, B, C}`) that
holds a `SceneManager<State>`, plus `Scene` and `LazyScene` leaves. The observable
is that `activate()` calls `setEnabled(true)` on the mounted scene and
`deactivate()` calls `setEnabled(false)` on the departing one:

- `test_start_and_navigate` — `bind` scenes, `attach(owner)`, `start(A)` mounts A
  (`isActive(A)`, A enabled). `owner.setState(B)` deactivates A and mounts B.
- `test_history_back` — after `start(A)` → `setState(B)`, `back()` returns to A:
  the owner is back in state A, A is mounted, B is unmounted.
- `test_lazy_construction` — `bindLazy<LazyScene>(C)` does **not** construct the
  scene at bind time; navigating to C constructs it exactly once (verified via a
  static ctor counter). `has(C)` is true once bound.

## How it works

Window-free. Navigation always goes through the owner's state machine
(`setState` / `onStateEnter` / `onStateExit`), which is how `SceneManager` hooks
in via `attach`. Scene mounting is observed through the enabled flag rather than
rendering. See [`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `SceneManager: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A fail means a scene was not
mounted/unmounted on a transition, `back()` history is wrong, or a lazy scene was
built at the wrong time (or more than once).
