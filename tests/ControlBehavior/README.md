# ControlBehavior

Locks the **value semantics** of the widget library — the data side of the
controls. Complements `ControlCascade`, which covers their enable/disable
behavior. Uses real controls with the default font, still window-free.

## What's tested

- `test_select_selection` — `Select`: `addOption(label, value)`, `optionCount()`,
  `selectIndex(i)`, and that `getSelectedIndex` / `getSelectedValue` /
  `getSelectedLabel` reflect the current pick (selection moves, not accumulates).
- `test_radiogroup_single_select` — `RadioGroup`: adding options and selecting;
  picking a second option replaces the first (single-select exclusivity), and
  `getSelected()` returns the live button.
- `test_list_contents` — `List`: `addItem`, `rowCount`, `removeAt`, `clear`
  bookkeeping.
- `test_editablelist_values` — `EditableList` in Single mode: `setValues(vals,
  selected)` then `values()` / `selectedIndices()` round-trip; `setSelected`
  replaces the selection under single-select.
- `test_textinput_value` — `TextInput`: `setValue` / `getValue`, a rich-text JSON
  round-trip (`getRichText` → `setRichText` preserves the plain text), and
  `clear()`.

## How it works

Window-free; controls built with `FontManager<>::getDefault()`. These tests read
and write control *data* — no rendering, no event dispatch. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `ControlBehavior: all checks passed` and exits `0`. A failure prints
the offending `CHECK` with `file:line` and exits `1`. A fail means a control's
public value API is wrong — e.g. selection not updating, list counts drifting, or
text not surviving a serialize/restore round-trip.
