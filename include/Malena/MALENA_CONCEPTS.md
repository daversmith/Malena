# Malena Framework Concepts

This document explains the key ideas behind the Malena framework.

Malena is a C++ framework built on top of SFML that promotes an event-driven architecture and modular design through traits, managers, and reusable graphical components.

---

## Event-Driven Architecture

A central goal of Malena is to make SFML applications feel more event-driven.

Instead of objects tightly referencing each other, communication occurs through two complementary systems:

### EventsManager — string-keyed pub-sub

`EventsManager` is the central event bus. Components subscribe to named events and receive callbacks when those events fire. The `UIManager` polls SFML input each frame and translates it into Malena event names:

```
SFML Input Event
  → UIManager (translates + filters)
    → EventsManager::fire("click" / "hover" / "update" / ...)
      → subscribed component callbacks
```

Built-in event names: `"click"`, `"hover"`, `"unhover"`, `"focus"`, `"blur"`, `"update"`.

Subscriptions are registered through the `Messenger` trait methods (`onClick`, `onHover`, etc.) rather than the raw `EventsManager` API.

**Safety:** `EventsManager` tracks a `fireDepth` counter while iterating. Any `unsubscribe` call made during a callback is queued and processed only after the current fire completes. This prevents iterator invalidation and crash-on-unload bugs.

### MessageManager — typed enum-keyed messages

`MessageManager` handles structured communication between framework objects — for example, a plugin telling another plugin that a loading screen has finished, or a game notifying a launcher to stop. Messages are keyed by a user-defined enum and carry a typed payload:

```cpp
// Sender
sendMessage<bool>(MyEvent::Loaded, true);

// Receiver
onMessage<bool>(MyEvent::Loaded, [](const bool& loaded) {
    if (loaded) { /* proceed */ }
});
```

Like `EventsManager`, `MessageManager` uses a deferred removal pattern so that unsubscribing inside a message callback is always safe.

---

## Traits

Traits are reusable behavior modules that can be composed into classes.

Instead of deep inheritance hierarchies, Malena uses traits to attach capabilities to objects.

### Messenger

The most commonly used trait. Wraps `EventsManager` subscriptions with named convenience methods:

```cpp
rect.onClick([]{ /* clicked */ });
rect.onHover([]{ /* hovered */ });
rect.onUpdate([]{ /* every frame */ });
```

`Component` inherits `Messenger` and automatically calls `unsubscribeAll()` in its destructor, so there are no dangling callbacks when a component is destroyed.

### Flaggable / CustomFlaggable

Provides runtime boolean flag management. Flags are declared in the class manifest:

```cpp
setFlag(MyManifest::Flags::Active);
checkFlag(MyManifest::Flags::Active);
clearFlag(MyManifest::Flags::Active);
```

### Draggable

Adds mouse-drag interaction to any UI component.

### Positionable

Provides a consistent positional API (`setPosition`, `getPosition`, relative placement) across framework objects.

### Subscribable

Lower-level subscription infrastructure used by `Messenger`.

### Customizable

Supports manifest-declared visual customization (colors, sizes, etc.).

---

## Deferred Operations

One of Malena's most important behind-the-scenes mechanisms.

Any time a manager (`EventsManager`, `MessageManager`, `ComponentsManager`, `PluginManager`) is actively iterating its internal collection, destructive operations — unsubscribes, component removals, plugin unloads — are **queued** rather than executed immediately. They are then processed at the next safe point (typically at the start of the next public call on that manager).

This means user code can safely unsubscribe inside a callback, remove a component from a click handler, or unload a plugin from a message handler without risking crashes. The framework handles the timing automatically.

---

## The `With<Manifest>` Pattern

The `With<Manifest>` template is how manifest metadata is applied to a class. It is the primary pattern for creating typed, resource-aware framework objects:

```cpp
// A Component that knows about its own textures and flags
class MyWidget : public ml::ComponentWith<MyManifest> { ... };

// A plugin with manifest-declared resources and metadata
class MyPlugin : public ml::PluginWith<MyPlugin::Manifest> { ... };
REGISTER_PLUGIN(MyPlugin)
```

The template wires manifest-declared resources into the appropriate managers at compile time, so you get type-safe access without manual boilerplate.

---

## Managers

Managers coordinate shared framework systems. All managers that own collections use the deferred operations pattern for safety.

Examples:

| Manager | Responsibility |
|---|---|
| `ComponentsManager` | Tracks registered UI components; calls update/draw |
| `WindowManager` | Owns the SFML RenderWindow |
| `PluginManager` | Loads, tracks, and safely unloads plugins |
| `MessageManager` | Typed enum-keyed messaging |
| `EventsManager` | String-keyed event pub-sub |
| `TextureManager` | Texture loading and reuse |
| `FontManager` | Font loading and reuse |
| `SoundManager` | Sound buffer loading and reuse |
| `ConfigManager` | Key-value configuration |
| `FlagManager` | Runtime flag storage |
| `StateManager` | Enum-keyed state storage |
| `AssetsManager` | Combines Texture + Font + Sound managers |

---

## Graphics Layer

The `Graphics` module provides reusable visual components built on top of SFML.

All graphics components inherit from `Component`, which provides:

- the `Messenger` trait (click, hover, update, etc.)
- automatic unsubscription on destruction
- integration with `ComponentsManager` for update/draw

Components include:

- **Primitives:** `Circle`, `Rectangle`, `RoundedRectangle`, `Convex`, `VertexArray`
- **Sprites:** `Sprite` (framework-wrapped with manifest resource support)
- **Text:** `Text`, `TextBox`, `TextInput`, `Typer`, `Cursor`
- **Controls:** `Button`, `CircleButton`, `RectangleButton`, `ConvexButton`, `Carousel`
- **Layouts:** `Grid`

**Critical constraint:** Components cannot be safely copied or moved after they have been registered with a manager. Use `std::unique_ptr` for collections of components owned by a class.

---

## Resources

The `Resources` module manages shared data such as textures, fonts, sounds, configuration, flags, and states.

Centralized resource management means multiple objects can share loaded assets without duplication. Resources are declared in manifests and accessed through the appropriate manager.

---

## Manifests

Manifests provide declarative metadata for classes and plugins.

A manifest is a nested struct that declares:

- resource paths (textures, fonts, sounds)
- custom flag enums
- custom state enums
- configuration keys
- plugin name and version

**Manifest hierarchy — choose the right level:**

| Level | What it provides | Best used when |
|---|---|---|
| Individual manager (e.g., `TextureManager<M>`) | One asset type | You only need textures or only fonts |
| `AssetsManager<M>` | All media (textures + fonts + sounds) | Most visual components and game plugins |
| `Context<M>` | Assets + ConfigManager | Complex apps or plugins with both media and runtime config |

---

## Plugins

The plugin system allows Malena applications to extend functionality through dynamically loaded modules.

### Plugin base types

- `Plugin` — base class; provides `onLoad()` and `onUnload()` hooks
- `PluginWith<Manifest>` — adds manifest support; used for virtually all real plugins
- `REGISTER_PLUGIN(ClassName)` — macro that exports the factory function the `PluginManager` calls when loading the shared library

### Plugin lifecycle

```
loadPlugin(path) → factory() → onLoad() → [in use] → unloadPlugin() → [deferred] → onUnload() → delete → dlclose()
```

Unloading is always deferred until the current iteration context is complete, preventing use-after-free when a plugin triggers its own unload.

### Querying plugin interfaces

Because plugins cross dylib boundaries, use `getIf<InterfaceType>()` to safely query optional interfaces rather than relying on `dynamic_cast`:

```cpp
if (auto* drawable = plugin->getIf<ml::Component>()) {
    addComponent(*drawable);
}
```

---

## ECS Support

Malena includes an optional Entity Component System architecture.

The ECS module allows developers to build systems where:

- entities hold data components
- systems process all entities with matching components
- logic is separated from data

Built-in systems include `HealthSystem`, `MovementSystem`, `RenderingSystem`, and `SizeSystem`.

This supports data-oriented design patterns common in games and simulations.

---

## Relationship to SFML

Malena is built on top of SFML and preserves SFML idioms.

SFML provides: windowing, graphics rendering, audio, input events, textures and fonts.

Malena adds: application structure, event-driven communication, reusable UI components, plugin architecture, resource management, and trait-based behavior composition.

Malena is therefore best understood as a **framework layer on top of SFML** rather than a replacement for it. Code that uses raw SFML types and idioms (e.g., `sf::Vector2f`, `window.draw()`) continues to work as expected.