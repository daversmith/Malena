# Malena test suite

A set of **window-free regression suites** that lock the framework's behavioral
invariants — the subtle event / enable / hit-test / topology rules whose
violation has historically produced hard-to-find bugs (e.g. the "toolbar needs
two clicks" class).

Each suite is a standalone executable in its own folder; each folder has a
`README.md` describing what it covers.

## How the suites work (shared mechanics)

- **No window, no GL, no assets.** Every suite constructs real framework objects
  but never opens a window or renders. Controls that need a font use
  `FontManager<>::getDefault()`. This keeps the suites fast and runnable in CI /
  headless. Anything that genuinely needs a GPU (actual `draw()` output, pixel
  layout) is intentionally **out of scope** here.
- **Plain `CHECK` macro.** There is no external test framework. Each `main.cpp`
  defines a tiny macro:
  ```cpp
  #define CHECK(cond) do { if (!(cond)) { \
      std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
      ++failures; } } while (0)
  ```
  Every assertion is a `CHECK(...)`. The program counts failures and returns a
  non-zero exit code if any failed — which is what CTest keys on.
- **One executable per subsystem.** `tests/CMakeLists.txt` builds each folder's
  `main.cpp` into `<Name>Test` and registers it with `add_test(NAME <Name> ...)`.

## Building and running

Tests are off by default; enable them with `MALENA_BUILD_TESTS`:

```bash
cmake -S . -B build-tests -DMALENA_BUILD_TESTS=ON
cmake --build build-tests
cd build-tests && ctest --output-on-failure
```

Run a single suite directly (useful when iterating):

```bash
cmake --build build-tests --target FlagsTest
./build-tests/tests/FlagsTest
```

## What a pass / fail looks like

**Pass** — each suite prints a single success line and exits `0`:

```
Flags: all checks passed
```

Under CTest:

```
12/12 Test #12: ManifestResources ... Passed
100% tests passed, 0 tests failed out of 12
```

**Fail** — the offending `CHECK` prints its condition and source location, the
suite prints a count and exits `1`:

```
FAIL: s.getSelectedValue() == "v1" at .../ControlBehavior/main.cpp:34
ControlBehavior: 1 check(s) failed
```

CTest then reports that suite as `***Failed`. The printed condition + `file:line`
is enough to jump straight to the broken invariant.

## The suites

| Suite | Subsystem it locks |
|-------|--------------------|
| `ChildOrdering`     | Layer-key child ordering + `addComponents` variadics |
| `EnableCascade`     | enable/disable cascade through composite children |
| `HitTestInvariants` | the three gates the click filter composes |
| `ControlCascade`    | per-control enable/disable + container cascades |
| `CoreTopology`      | reparent / remove / effective-visibility / `topmostMatching` |
| `Flags`             | `FlagManager` API + custom manifest Flag/State |
| `Positionable`      | position, bounds, layout helpers, move queue |
| `EventSystem`       | subscribe / publish / filter / exclusive owner |
| `Messaging`         | typed `MessageManager` channels |
| `SceneManager`      | bind / start / navigate / back / lazy scenes |
| `ControlBehavior`   | control value semantics (selection, values, text) |
| `ManifestResources` | manifest registry + default-font singleton |

## Adding a suite

1. `mkdir tests/MyThing` and write `tests/MyThing/main.cpp` (copy the `CHECK`
   macro and the `main()` return convention from any existing suite).
2. Add `MyThing` to the `foreach(suite ...)` list in `tests/CMakeLists.txt`.
3. Reconfigure (`cmake -S . -B build-tests -DMALENA_BUILD_TESTS=ON`) and run.
4. Add a `README.md` to the new folder.
