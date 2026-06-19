# EventSystem

Locks the **event subscription bus** (`Subscribable` → `EventManager`) and the
**exclusive-owner registry** (`AppManager`) that the click filter gates on.

This is the string-keyed *event* channel (UI input, `onClick`-style callbacks),
distinct from the typed *message* channel covered by the `Messaging` suite.

## What's tested

Using `Node : Component<>` instances that count callback invocations, and a
custom `enum class Ev { Ping }`:

- `test_publish_delivers` — `subscribe(Ev::Ping, cb)` then `publish(Ev::Ping)`
  invokes the callback; publishing again invokes it again.
- `test_publish_multi_subscriber` — a `publish` reaches **every** subscriber of
  that event, not just the publisher.
- `test_unsubscribe_all` — `unsubscribeAll()` stops delivery to that node while
  others keep receiving.
- `test_publish_filter` — `publish(event, filter)` with a rejecting filter
  suppresses the fire; an accepting filter delivers.
- `test_exclusive_owner_registry` — `setExclusiveOwner` / `clearExclusiveOwner` /
  `isUnderExclusiveOwner`: with no owner everyone passes; with an owner set, the
  owner passes and unrelated nodes are locked out; clearing re-opens everyone.

## How it works

Window-free; uses the `Subscribable` API on real components plus the static
`AppManager` owner calls. Each test calls `unsubscribeAll()` / `clearExclusiveOwner()`
so suites don't leak global subscriptions into one another. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `EventSystem: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A fail means events are
mis-delivered (missed, duplicated, or reaching the wrong nodes) or the
exclusive-owner gate — which blocks the background while a dropdown/modal is open
— is leaking.
