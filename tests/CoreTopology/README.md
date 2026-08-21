# CoreTopology

Locks the **per-instance parent/child topology** (`_children` + `_parent`) that
replaced the old static `_childMap`, plus the `topmostMatching` hit-test seam.

## What's tested

Using plain `Component<>` leaves:

- `test_reparent_moves_child` — Malena is **single-parent**: `addComponent`ing a
  child that already has a parent silently moves it. Verified by cascade —
  disabling the old parent no longer reaches it; disabling the new one does.
- `test_remove_detaches` — after `removeComponent`, the cascade no longer reaches
  the (former) child.
- `test_effective_visibility_chain` — `isEffectivelyVisible()` walks the ancestor
  chain: hiding a grandparent makes a child effectively invisible while the
  child's *own* `isVisible()` flag stays untouched.
- `test_topmost_matching` — `topmostMatching(accept)` returns the **front-most**
  (last-painted, highest-layer) node first, **skips invisible** subtrees, and
  honors a predicate that accepts only a specific node.

## How it works

Window-free; observes topology through the enable cascade and the public
`isEffectivelyVisible()` / `topmostMatching()` API — no rendering. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `CoreTopology: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A fail means parent/child
bookkeeping is wrong — which would corrupt cascades, draw order, and click
routing across the whole framework.
