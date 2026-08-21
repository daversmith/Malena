# ChildOrdering

Locks the **layer-key child ordering** guarantees of `Core`'s per-instance
`_children` vector. When a component registers children with `addComponent`, the
draw/iteration order must be **layer ascending, registration order within a
layer** — that ordering is what makes z-stacking (backgrounds behind content
behind overlays) predictable.

## What's tested

Through a thin `Component<>` probe that exposes `getChildren()`:

- `test_explicit_layer_orders_ascending` — children added at layers 0/100/200
  come back sorted ascending.
- `test_within_layer_keeps_insertion_order` — same-layer children keep insertion
  order (stable sort).
- `test_default_layer_is_100` — a no-layer `addComponent` lands at the default
  layer 100, between a 0 and a 200.
- `test_no_arg_addComponent_still_orders_by_insertion` — all-default still keeps
  insertion order.
- `test_readding_same_child_updates_layer` — re-adding a child updates its layer
  (single-parent move) rather than duplicating it.
- `test_addComponents_variadic_default_layer` / `..._shared_layer` — the
  variadic `addComponents(...)` / `addComponents(layer, ...)` helpers register
  each argument at the right layer.
- enum-overload tests — `addComponent(child, SomeEnum::Value)` uses the enum's
  underlying int as the layer key.

## How it works

Window-free; inspects `getChildren()` directly (no rendering). See
[`../README.md`](../README.md) for the shared `CHECK` macro and build/run steps.

## Pass / fail

Pass prints `ChildOrdering: all checks passed` and exits `0`. A failure prints
`FAIL: <condition> at .../ChildOrdering/main.cpp:<line>` and exits `1` — a fail
here means the child list is mis-ordered, which would corrupt draw order and
hit-testing for any composite component.
