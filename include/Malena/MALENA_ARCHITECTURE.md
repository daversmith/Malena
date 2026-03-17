# Malena Framework Architecture

## Overview

Malena is a C++ framework built on top of SFML that emphasizes event-driven architecture, modular composition, and reusable graphical components.

The framework is designed to simplify application and UI development by wrapping common SFML workflows in higher-level systems for application management, messaging, plugins, resources, traits, and graphics. Rather than forcing logic into tightly coupled class hierarchies, Malena encourages communication through events and messages, behavior through traits, and extensibility through manifests and plugins.

At a high level, Malena can be understood as a framework with the following goals:

- provide a clean application entry point for SFML-based programs
- support event-driven communication between framework objects
- encourage modular design through traits and plugins
- simplify management of resources, flags, states, and assets
- provide reusable graphics and UI controls
- support both object-oriented and ECS-based development styles

## Core Philosophy

### Event-driven architecture

Malena makes SFML development feel more event-driven. Instead of requiring objects to be tightly aware of one another, the framework supports communication through two complementary systems: an **event system** (input and lifecycle events, fired by name) and a **message system** (typed, enum-keyed communication between plugins and framework objects). Together these allow interactive UIs and plugin-based architectures to be built without direct object dependencies.

### Composition over rigid inheritance

Malena uses traits to compose behavior into objects. Capabilities such as messaging, flags, dragging, positioning, and customization are modeled as reusable traits rather than being repeated across unrelated classes. The `With<Manifest>` template pattern (e.g., `UIComponentWith<Manifest>`, `PluginWith<Manifest>`) layers manifest-declared resources, flags, and states onto any framework class cleanly.

### Safety through deferred operations

A fundamental concern in event-driven and plugin-based systems is mutation during iteration — for example, an event callback unloading a plugin while the event system is still iterating its subscriber list. Malena addresses this through a consistent **deferred operations pattern**: `EventsManager`, `MessageManager`, `ComponentsManager`, and `PluginManager` all track when they are actively iterating and queue destructive operations (removals, unloads, unsubscribes) to be processed safely afterward. User code does not need to manage this manually.

### Framework-level abstraction over SFML

SFML remains the rendering and windowing foundation, but Malena provides a higher-level structure for application flow, window ownership, graphics components, and interaction patterns.

### Extensibility through manifests and plugins

Malena supports configurable and extensible systems through manifests, plugin metadata, and plugin management. This allows applications and modules to declare resources, states, flags, and plugin information in a structured way.

---

## High-Level Architecture

The framework is organized into the following major subsystems:

### Core

The `Core` module provides foundational framework abstractions shared across the rest of Malena.

Important responsibilities include:

- defining common base types such as `Core`, `Component`, and `Manager`
- adapting framework objects through `CoreAdapter`
- managing collections of core objects through `CoreManager` / `ComponentsManager`
- supporting deferred changes through `DeferredOperationsManager`
- wrapping drawable behavior through `DrawableWrapper`

The `Core` layer acts as the backbone of the framework. Many higher-level systems either inherit from or coordinate through these foundational types.

**Critical constraint:** Malena components cannot be safely copied or moved after they have been registered with a manager. Prefer `std::unique_ptr` collections when you need to manage component lifetimes manually (e.g., `std::vector<std::unique_ptr<Tile>>`).

---

### Engine

The `Engine` module contains the main runtime infrastructure of the framework.

#### App

The `Engine/App` subsystem contains the main application model:

- `Application` — the high-level entry point for Malena programs. Subclasses override `initialization()` and `registerEvents()` to set up components and subscriptions.
- `AppManager` — coordinates the application lifecycle and main loop, including window polling, event distribution, update ticks, and drawing.
- `Controller` and `UIController` — control-layer abstractions for event registration and user interaction flow. `UIController` is the primary base for applications that host UI components.

The two virtual methods that application authors implement are:

```cpp
void initialization() override;  // called once at startup; add components here
void registerEvents() override;   // called after initialization; set up subscriptions here
```

#### Events

The `Engine/Events` subsystem contains `EventsManager`, which is the centralized publish-subscribe event bus for framework-level events.

**How it works:**

- Components subscribe to named events (e.g., `"click"`, `"hover"`, `"update"`) via the `Messenger` trait.
- `UIManager` polls SFML events each frame, translates them into Malena event names, and calls `EventsManager::fire()`.
- `EventsManager` iterates its subscriber map and invokes registered callbacks.
- A `fireDepth` counter tracks nested firing. Any unsubscribe calls made during firing are queued and processed safely once `fireDepth` returns to zero. This prevents iterator invalidation crashes.

**Event names fired by UIManager:**
- `"click"` — mouse button released over a component that was also pressed
- `"hover"` — mouse entered a component's bounds
- `"unhover"` — mouse left a component's bounds
- `"focus"` / `"blur"` — keyboard focus gained/lost
- `"update"` — fired every frame

#### Messaging

The `Engine/Messaging` subsystem contains `MessageManager`, which provides **typed, enum-keyed messaging** distinct from the string-based event system.

Where `EventsManager` handles input-driven events fired by name, `MessageManager` handles structured communication between framework objects — for example, a plugin signaling another plugin that loading is complete, or a game notifying the launcher to stop. Messages are keyed by enum value and carry a typed payload.

Like `EventsManager`, `MessageManager` uses the deferred operations pattern internally so that unsubscription during a message callback is safe.

#### Plugins

The `Engine/Plugins` subsystem contains:

- `Plugin` — base class for all dynamically loaded plugin types
- `PluginWith<Manifest>` — the primary template authors use; combines `Plugin` with manifest-declared resources, flags, states, and metadata
- `PluginInfo` — metadata and handle storage for a loaded plugin
- `PluginManager` — loads, tracks, and unloads plugins

**Plugin lifecycle:**

1. `PluginManager::loadPlugin(path)` opens a shared library, calls the factory function, and stores the result.
2. `Plugin::onLoad()` is called — the plugin initializes itself.
3. The plugin is available via `getPlugins()` and `getPlugin<T>()`.
4. When `unloadPlugin(plugin)` is called, the actual unload is **deferred** until the next safe point (i.e., after any active iteration completes). At that point `Plugin::onUnload()` fires, the plugin is deleted, and the library handle is closed.

**Registration:** Every plugin shared library must define a factory function via the `REGISTER_PLUGIN(ClassName)` macro, which the `PluginManager` looks up by convention when loading.

**RTTI across dylib boundaries:** Plugins loaded from separate shared libraries share only a virtual interface with the host. To query a plugin for optional interfaces (e.g., whether it is drawable), use `getIf<InterfaceType>()` rather than `dynamic_cast` across dylib boundaries.

#### Window

The `Engine/Window` subsystem contains `WindowManager`, which centralizes ownership of the SFML `RenderWindow`. This ensures the window remains accessible across the framework without manual passing.

---

### Graphics

The `Graphics` module contains Malena's reusable visual and UI-oriented components built on top of SFML drawing primitives. All graphics components inherit from `UIComponent`, which provides the `Messenger` trait, auto-unsubscription on destruction, and integration with `EventsManager` and `ComponentsManager`.

#### Base

- `Graphic` — abstract base for all visual framework objects
- `Button` — base interactive control combining a background shape with click/hover behavior

#### Controls

- `Carousel` — scrollable container for presenting a collection of visual items; supports shader effects
- `CircleButton`, `ConvexButton`, `RectangleButton` — shaped interactive buttons

#### Layouts

- `Grid` — layout container that positions children in rows and columns

#### Primitives

- `Circle`, `Convex`, `Rectangle`, `RoundedRectangle`, `VertexArray` — framework-wrapped SFML shapes with trait and manager integration

#### Sprites

- `Sprite` — framework-wrapped SFML sprite with manifest and resource system integration

#### Text

- `Text`, `TextBox`, `TextInput`, `Typer`, `Cursor` — a rich text subsystem supporting static display, multi-line layout, interactive input, and animated typing effects

---

### ECS

The `ECS` module provides an entity-component-system architecture as an alternative or complement to the framework's object-oriented systems.

#### ECS/Core

- `ECSManager`, `EntitiesAndComponents`, `EventManagerECS` — data and management layer

#### ECS/System

- `HealthSystem`, `MovementSystem`, `RenderingSystem`, `SizeSystem` — built-in systems for common game logic

The ECS subsystem allows Malena to support data-oriented patterns where logic operates over entities and components through systems rather than through deep inheritance trees.

---

### Manifests

The `Manifests` module provides declarative configuration and metadata support.

A manifest is a nested struct inside a class that declares the resources, flags, states, and configuration data that class needs. The `With<Manifest>` template pattern then wires those declarations into the manager infrastructure automatically.

Key types:

- `Manifest` — base manifest concept
- `DefaultManifest` — used when a class needs no custom metadata
- `ManifestAliases` — convenience aliases for manifest-declared types
- `Context` — couples an `AssetsManager` (textures, fonts, sounds) with a `ConfigManager`; the highest-level manifest grouping
- `Resources` — manifest-level resource declarations

**Manifest hierarchy:**

| Level | What it provides | Best used when |
|---|---|---|
| Individual manager (e.g., `TextureManager<Manifest>`) | One media type | You only need textures or only fonts |
| `AssetsManager<Manifest>` | All media types (textures + fonts + sounds) | Most game or UI classes |
| `Context<Manifest>` | Assets + configuration | Complex apps or plugins with both media and config |

---

### Resources

The `Resources` module manages shared data and runtime assets.

Important managers include:

- `AssetsManager` — combines TextureManager, FontManager, SoundManager into one interface
- `ConfigManager` — key-value configuration data declared in manifests
- `FlagManager` — runtime flag storage for `Flaggable` objects
- `FontManager`, `TextureManager`, `SoundManager` — individual media type managers
- `ResourceManager` — base manager shared infrastructure
- `StateManager` — enum-keyed state storage

---

### Traits

The `Traits` module is one of the most important architectural features of Malena. Traits provide reusable behavior that can be mixed into framework classes, supporting composition-based design and reducing duplication.

#### Messenger

The primary trait for event-driven interaction. Provides named convenience methods over the raw `EventsManager` subscription API:

```cpp
onClick([]{});             // fires on mouse click
onHover([]{});             // fires when mouse enters bounds
onUnhover([]{});           // fires when mouse leaves bounds
onFocus([]{});             // fires on keyboard focus gain
onBlur([]{});              // fires on keyboard focus loss
onUpdate([](sf::Event){}); // fires every frame
```

`UIComponent` inherits `Messenger` and auto-calls `unsubscribeAll()` in its destructor, preventing dangling callbacks after a component is destroyed.

#### Subscribable

Lower-level subscription infrastructure underlying `Messenger`.

#### Flaggable / CustomFlaggable

Provides runtime boolean flag management. Flags are declared in the class manifest and can be checked, set, and cleared:

```cpp
setFlag(MyFlags::Active);
checkFlag(MyFlags::Active);
clearFlag(MyFlags::Active);
```

`CustomFlaggable` supports flags declared directly on a class's manifest without a shared enum.

#### Draggable

Enables mouse-drag interaction for any UI component.

#### Positionable

Provides a consistent positional API across framework objects.

#### Customizable

Supports manifest-declared visual customization (colors, sizes, etc.).

#### Trait Infrastructure (`Traits/Base`)

- `Trait` — base concept for all traits
- `MultiCustomFlaggable`, `MultiCustomStateManager` — support for multiple custom flag/state sets on a single class

---

### Utilities

The `Utilities` module contains shared helper types and functions:

- alignment helpers (`ml::Align`)
- enum hashing
- mouse event helpers
- shader utilities
- tweening
- texture slicing
- type extraction

---

## Relationship to SFML

Malena is built on top of SFML, not intended to replace it.

SFML provides: render windows, drawing, events, textures, fonts, sound, and low-level multimedia primitives.

Malena adds: application structure, event-driven communication patterns, reusable UI controls, plugin infrastructure, manifest-driven customization, centralized resource and state systems, and optional ECS architecture.

In practice, Malena can be understood as a framework layer that organizes SFML applications into a more modular and event-oriented design while preserving SFML idioms (e.g., `window.draw()` still works as expected).

---

## Execution Model

A typical Malena application follows this flow:

1. An `Application` subclass is instantiated.
2. `WindowManager` takes ownership of the `sf::RenderWindow`.
3. `AppManager` enters the main loop.
4. Each frame, `UIManager` polls SFML events and fires corresponding Malena events through `EventsManager`.
5. `MessageManager` delivers any queued typed messages.
6. Subscribed callbacks on `UIComponent` instances are invoked via their `Messenger` trait.
7. `ComponentsManager` calls `update()` and `draw()` on all registered components.
8. Deferred operations accumulated during the frame (unsubscribes, component removals, plugin unloads) are processed at safe points — typically at the start of the next public method call on the relevant manager.

This flow allows interactive behavior while keeping systems decoupled and safe from mutation-during-iteration crashes.

---

## Architectural Roles of Key Types

| Type | Role |
|---|---|
| `Application` | Framework entry point; subclassed by users |
| `AppManager` | Main loop coordinator |
| `UIController` | Control-layer abstraction; base for UI-heavy apps |
| `UIComponent` | Base visual class; inherits `Messenger`; auto-unsubscribes on destruction |
| `EventsManager` | Centralized string-keyed pub-sub bus with deferred removal |
| `MessageManager` | Typed enum-keyed messaging with deferred removal |
| `PluginManager` | Loads, queries, and safely unloads plugins |
| `Plugin` / `PluginWith<M>` | Plugin base; `PluginWith` adds manifest support |
| `CoreManager` / `ComponentsManager` | Manages registered framework objects with deferred removal |
| `DeferredOperationsManager` | Shared deferred-operation infrastructure used by all managers |
| `Manifest` / `Context` | Declarative resource and config metadata |
| `Messenger` trait | Convenience subscription API for events |
| `Flaggable` trait | Runtime boolean flags declared in manifests |

---

## Design Strengths

- clear subsystem separation with consistent patterns across managers
- deferred operations model prevents crashes from mutation-during-iteration
- strong composition support through traits and `With<Manifest>` templates
- explicit event-driven design with two complementary communication channels (events and messages)
- reusable graphics and controls built on SFML
- modular plugin extension with safe lifecycle management
- optional ECS support for data-oriented workflows
- centralized resource and state management

---

## Intended Use Cases

Malena is especially well-suited for:

- SFML-based graphical applications and UI tools
- educational game and interactive application development (including the DaveStation platform)
- modular applications with dynamically loaded plugins
- projects that benefit from event-driven interaction without tight coupling
- hybrid object-oriented and ECS-based architectures

---

## Suggested Mental Model for Contributors

- `Core` — shared foundation and deferred operations infrastructure
- `Engine` — application lifecycle, event bus, message bus, plugin management
- `Graphics` — visual and UI building blocks on top of SFML
- `Traits` — mixin behavior (messaging, flags, drag, position)
- `Resources` / `Manifests` — declarative configuration and shared data
- `Plugins` — modular, safely-unloadable extensions
- `ECS` — optional data-oriented architecture

---

## Documentation Guidance

When documenting Malena code, public API comments should describe each class in terms of its role in this architecture. For example:

- describe `Application` as part of the application lifecycle and engine entry point
- describe `Messenger` as part of the event-driven communication model
- describe `Plugin` / `PluginWith` as part of the modular extension system, noting safe unloading
- describe graphics classes as reusable visual components built on SFML
- describe traits as mixin-style behavior providers
- describe `DeferredOperationsManager` as the crash-safety mechanism for all collection-mutating managers

---

## Future Expansion

This document should be refined over time with:

- event flow diagrams showing EventsManager and MessageManager side by side
- plugin lifecycle diagram (load → onLoad → use → deferred unload → onUnload → close)
- trait interaction examples
- `With<Manifest>` pattern cookbook
- ECS usage examples
- module-level Doxygen groups

As the framework evolves, this file should remain the main architectural reference for contributors, users, and AI-assisted documentation workflows.