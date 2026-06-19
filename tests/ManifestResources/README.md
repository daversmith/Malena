# ManifestResources

Locks the **static manifest registry** every component's manifest uses to map
enum keys to file paths / config values, and the **default-font singleton** that
controls fall back on when constructed without an explicit font.

## What's tested

Using a `TestManifest : ml::Manifest` populated in its static initializer
(`enum class Images { Background, Player }`, `enum class Ints { MaxPlayers }`):

- `test_manifest_filepaths` — `getFilepath(Images::Background)` returns the
  registered path; different enum keys address independent slots.
- `test_manifest_config` — `getConfig<Ints, int>(Ints::MaxPlayers)` returns the
  registered typed value (`4`).
- `test_default_font_singleton` — two calls to `FontManager<>::getDefault()`
  return the **same** `sf::Font` instance (a stable singleton, not a fresh copy
  per call). Controls call this in their default constructor, so it must be one
  shared font.

## How it works

Window-free. The manifest registry is pure data (no GL), and the default font is
a process-wide handle — reading its address needs no window. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `ManifestResources: all checks passed` and exits `0`. A failure
prints the offending `CHECK` with `file:line` and exits `1`. A fail means the
manifest stored/returned the wrong value for a key, or `getDefault()` is handing
out distinct font instances (which would waste memory and break shared styling).
