# Messaging

Locks the **typed message bus** (`MessageManager`) — the second of Malena's two
communication channels. Unlike the event system (string-keyed, delivers to
`Subscribable`s), messages are **typed payloads** delivered to any subscriber
identified by an opaque pointer, keyed by `(enum value, DataType)`.

## What's tested

Using `enum class Msg { Score, Name }` and the static `MessageManager` API
directly (the `Messenger` trait is thin sugar over this):

- `test_typed_delivery` — `subscribe<int>(Msg::Score, &token, cb)` then
  `publish<int>(Msg::Score, 42)` delivers `42`; a later `publish` delivers the
  new value; after `unsubscribe`, further publishes are not delivered.
- `test_multi_subscriber` — two subscribers on the same event both receive the
  message.
- `test_channel_separation` — channels are keyed by **both** the enum value and
  the payload type: a `publish<std::string>(Msg::Name, ...)` does not reach an
  `int` subscriber on `Msg::Score`, and vice versa.

## How it works

Window-free; uses the manager's static `subscribe` / `publish` / `unsubscribe`
with stack `int` tokens as opaque subscriber identities. Each test unsubscribes
its tokens so global state doesn't leak between tests. See
[`../README.md`](../README.md) for shared mechanics.

## Pass / fail

Pass prints `Messaging: all checks passed` and exits `0`. A failure prints the
offending `CHECK` with `file:line` and exits `1`. A fail means a typed message
was mis-routed — delivered to the wrong type/event channel, dropped, or still
delivered after unsubscribe.
