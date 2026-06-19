# EnableCascade

Locks the **enable/disable cascade** through composite components — the behavior
any container with member children (a `ChatWindow`, a panel, a dialog) relies on
after the move to a single `onEnabledChanged` hook.

The model is two flags per `Core`: `_selfEnabled` (what *you* set) and
`_parentEnabled` (inherited from an ancestor). Effective enabled = both. The key
property: re-enabling a parent must **not** force-enable a child you had
independently disabled.

## What's tested

Using a `Composite` (two leaf children registered via `addComponents`) and a
composite-of-composite:

- `test_cascade_disables_children` — disabling the parent disables both children;
  re-enabling restores them.
- `test_independent_child_disable_survives_parent_toggle` — a child you disabled
  on its own stays disabled across a parent disable→enable cycle (only its
  inherited `_parentEnabled` is touched, never its own intent).
- `test_nested_cascade` — `setEnabled` on the outer component propagates two
  levels deep.
- `addComponents` variadic registration behaves like N× `addComponent`.

## How it works

Window-free; exercises the cascade via `isEnabled()` on plain `Component<>`
leaves — no SFML resources. See [`../README.md`](../README.md) for shared
mechanics.

## Pass / fail

Pass prints `EnableCascade: all checks passed` and exits `0`. A failure prints
the offending `CHECK` with `file:line` and exits `1` — a fail means the cascade
either failed to reach a child or clobbered a child's independent disabled state
(the regression this suite guards).
