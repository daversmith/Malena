# Malena

**A lightweight, event-driven C++ UI framework built on top of SFML.**

[**malena.app**](https://malena.app) · [Tutorials playlist](https://www.youtube.com/playlist?list=PLAK6gn2iUWAaOaDcVA5a3A7SFMW8ugor0) · [MIT License](#license)

---

- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [Application](#application)
- [UI Components](#ui-components)
- [Events and Messaging](#events-and-messaging)
- [Flags](#flags)
- [Positioning and Layout](#positioning-and-layout)
- [Manifest System](#manifest-system)
- [Context — App-wide Resources](#context--app-wide-resources)
- [Plugins](#plugins)
- [Best Practices](#best-practices)
- [Tutorials](#tutorials)
- [License](#license)

---

## Overview

Malena gives you a clean, component-oriented layer on top of SFML so you can focus on building your application instead of managing window loops, input dispatch, and asset loading.

**What Malena provides:**

- An `Application` shell that opens a window and runs your event/update/draw loop
- A publish/subscribe event system with per-component helper callbacks
- A complete set of UI graphics components that wrap SFML drawables
- Relative positioning and tweened animation helpers
- A **Manifest** system for type-safe, auto-loaded assets and configuration
- A **Context** façade that exposes one manifest's resources globally across your app
- A **Plugin** system for building modular, dynamically-loaded extensions

---

## Requirements

| Requirement | Version |
|-------------|---------|
| C++ standard | C++17 or newer |
| CMake | 3.14+ |
| SFML | 3.x |

---

## Installation

### Clone

```bash
git clone https://github.com/daversmith/Malena.git
```

### Build the bundled example

```bash
cd Malena
cmake -B build -S . -DMALENA_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/BasicExample/BasicExample
```

### Add as a subdirectory

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(MyApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/Malena)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Malena::Malena)
```

### FetchContent

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyApp LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    malena
    GIT_REPOSITORY https://github.com/daversmith/Malena.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(malena)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Malena::Malena)
```

---

## Quick Start

```cpp
#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/RectangleButton.h>

class MyApp : public ml::Application
{
    ml::RectangleButton button;

public:
    MyApp() : ml::Application(sf::VideoMode({800, 480}), "My App") {}

    void initialization() override
    {
        button.setSize({240, 64});
        button.setPosition({40, 40});
        button.setString("Click me");
        addComponent(button);
    }

    void registerEvents() override
    {
        button.onClick([this]{ button.setString("Clicked!"); });
    }
};

int main() { MyApp().run(); }
```

**Where to put code:**
- `initialization()` — create components, call `addComponent(...)`
- `registerEvents()` — attach `onClick`, `onHover`, `onUpdate`, custom `subscribe`

---

## Core Concepts

### Application shell
`ml::Application` combines `AppManager` (window, loop, draw) and `UIController` (your logic) into one base class. Override `initialization()` and `registerEvents()`, then call `run()`.

### Components
All UI objects inherit from `ml::Core`, which mixes in `Positionable`, `Flaggable`, `Subscribable`, and `Draggable`. Components are registered with `addComponent(...)` to be updated and drawn each frame.

### Events
String-keyed publish/subscribe system. Built-in events — `click`, `hover`, `unhover`, `focus`, `blur`, `keypress`, `textentered`, `mousemoved`, `update` — are fired automatically by the framework. You can publish your own events on any channel.

### Flags
Binary state bits like `HOVERED`, `FOCUSED`, `ENABLED`, `HIDDEN`. Many are updated automatically by the framework; you can check, set, and toggle them at any time.

### Manifests
A `Manifest` is a compile-time registry that maps `enum class` keys to asset file paths and configuration values. The framework loads and caches everything automatically — no manual `loadFromFile()` calls in component code.

---

## Application

### Constructors

```cpp
// Width, height, bit depth, and title
ml::Application(unsigned int w, unsigned int h, unsigned int bitDepth,
                const std::string& title);

// SFML VideoMode and title
ml::Application(const sf::VideoMode& mode, const std::string& title);

// With a separate UIController and an explicit render window
ml::Application(const sf::VideoMode& mode, const std::string& title,
                ml::UIController& controller,
                sf::RenderWindow& window = ml::WindowManager::getWindow());
```

Pass `*this` as the `UIController` argument when deriving from `Application` directly.

### ApplicationWith — with a manifest

When your application itself has global resources (an app icon, a default font, app-wide config), use `ml::ApplicationWith<MyManifest>`. It injects a `Resources` alias and all manifest enum aliases into scope automatically.

```cpp
class MyApp : public ml::ApplicationWith<MyManifest>
{
public:
    MyApp()
    : ml::ApplicationWith<MyManifest>(
        1280, 720, 32,
        Resources::get(Text::WindowTitle)) {}   // title from manifest

    void initialization() override
    {
        auto& icon = Resources::get(Images::Icon);
        auto& font = Resources::get(Fonts::Main);
        int   fps  = Resources::get(Ints::TargetFPS);
    }

    void registerEvents() override { ... }
};
```

### Lifecycle methods

| Method | Purpose |
|--------|---------|
| `initialization()` | Create components, call `addComponent(...)` |
| `registerEvents()` | Attach event callbacks |
| `run()` | Start the loop — poll → fire → update → draw |
| `reset()` | Tear down all events, messages, and components without closing the window |
| `clearEvents()` | Clear event subscriptions only |

---

## UI Components

All components live under `include/Malena/Graphics/`.

| Header | Description |
|--------|-------------|
| `Primitives/Rectangle.h` | Wraps `sf::RectangleShape` |
| `Primitives/Circle.h` | Wraps `sf::CircleShape` |
| `Primitives/Convex.h` | Wraps `sf::ConvexShape` |
| `Primitives/RoundedRectangle.h` | Rectangle with rounded corners |
| `Primitives/VertexArray.h` | Wraps `sf::VertexArray` |
| `Sprites/Sprite.h` | Wraps `sf::Sprite` |
| `Controls/RectangleButton.h` | Rectangular button with centered text |
| `Controls/CircleButton.h` | Circular button with centered text |
| `Controls/ConvexButton.h` | Convex-shape button with centered text |
| `Controls/Carousel.h` | Carousel / paged content control |
| `Text/Text.h` | Wraps `sf::Text`; optional word-wrap via `setWordWrap` / `setMaxWidth` |
| `Text/TextInput.h` | Composite text input field |
| `Text/TextBox.h` | Framed text display area |
| `Text/Typer.h` | Text entry logic (focus, `textentered`, `keypress`) |
| `Text/Cursor.h` | Insertion caret for text input |
| `Layouts/Grid.h` | Grid helper for cell-based placement |

### Examples

**RectangleButton**

```cpp
#include <Malena/Graphics/Controls/RectangleButton.h>

ml::RectangleButton btn;
btn.setSize({200, 50});
btn.setPosition({40, 40});
btn.setString("Submit");
addComponent(btn);

btn.onHover  ([&]{ btn.setFillColor(sf::Color(180, 220, 255)); });
btn.onUnhover([&]{ btn.setFillColor(sf::Color(100, 160, 255)); });
btn.onClick  ([&]{ /* handle click */ });
```

**Text and TextInput**

```cpp
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Graphics/Text/TextInput.h>

ml::Text      label;
ml::TextInput input;

label.setString("Username");
label.setPosition({40, 120});

input.setPosition({40, 150});
input.setSize({240, 32});

addComponent(label);
addComponent(input);

input.onTextEntered([&](const std::optional<sf::Event>&){ /* read input */ });
```

**Sprite**

```cpp
#include <Malena/Graphics/Sprites/Sprite.h>

ml::Sprite icon;
icon.setTexture(myTexture);
icon.setPosition({320, 60});
addComponent(icon);

icon.onClick([&]{ /* handle click */ });
```

**Grid**

```cpp
#include <Malena/Graphics/Layouts/Grid.h>

ml::Grid grid(3, 3);
grid.setPosition({40, 220});
grid.setCellSize({72, 72});
grid.setSpacing(8, 8);
addComponent(grid);
```

---

## Events and Messaging

### Built-in event helpers

Every component exposes these helpers:

```cpp
component.onClick     ([]{});
component.onHover     ([]{});
component.onUnhover   ([]{});
component.onFocus     ([]{});
component.onBlur      ([]{});
component.onUpdate    ([]{});
component.onKeypress  ([](const std::optional<sf::Event>&){});
component.onMouseMoved([](const std::optional<sf::Event>&){});
component.onTextEntered([](const std::optional<sf::Event>&){});
```

### Custom events

```cpp
// Subscribe
box.subscribe("MyEvent", []{ /* ... */ });

// Publish to all subscribers
ml::Messenger::publish("MyEvent");

// Publish with a filter predicate
ml::Messenger::publish("MyEvent", [](ml::UIComponent& c){
    return !c.checkFlag(ml::Flag::HIDDEN);
});

// Unsubscribe
box.unsubscribe("MyEvent");
box.unsubscribeAll();
```

---

## Flags

### Common flags

`HOVERED` · `CLICKED` · `FOCUSED` · `ENABLED` · `HIDDEN` · `BLINKING`

### Operations

```cpp
btn.checkFlag (ml::Flag::HOVERED);
btn.enableFlag(ml::Flag::ENABLED);
btn.disableFlag(ml::Flag::HIDDEN);
btn.toggleFlag(ml::Flag::FOCUSED);
btn.setFlag   (ml::Flag::ENABLED, true);
```

Many flags (`HOVERED`, `FOCUSED`, etc.) are maintained automatically by the framework. Check them inside `onUpdate` for continuous per-frame behaviour.

---

## Positioning and Layout

Every `ml::Core` object inherits `Positionable`, which provides both immediate placement and tweened animation.

### Immediate placement

```cpp
component.setPosition({100.f, 200.f});
sf::Vector2f     pos    = component.getPosition();
sf::FloatRect    bounds = component.getGlobalBounds();
```

### Relative layout helpers

```cpp
labelB.setRightOf (labelA, 10.f);   // place B 10 px to the right of A
button.setBelow   (header, 20.f);   // place button 20 px below header
button.setLeftOf  (panel,  8.f);
button.setAbove   (footer, 8.f);

icon.center            (panel);     // center on both axes
icon.centerHorizonally (panel);     // horizontal only
icon.centerVertically  (panel);     // vertical only
component.centerText   (myText);    // center an sf::Text inside this object
```

### Animated movement

```cpp
// Animate to an absolute position over 0.5 s
component.moveTo({400.f, 300.f}, 0.5f);

// Animate by a relative offset over 1 s (default)
component.moveDistance({50.f, 0.f});

// Poll animation state
if (component.isScrolling()) { ... }

// Set the assumed framerate for interpolation (default: 60)
component.setFramerate(120.f);
```

---

## Manifest System

A **Manifest** is a class that inherits `ml::Manifest`, declares inner `enum class` types for each category of resource it needs, and registers all paths and config values in a single inline static initializer.

```cpp
// MyManifest.h
#pragma once
#include <Malena/Manifests/Manifest.h>

class MyManifest : public ml::Manifest
{
public:
    // Required when used with PluginWith
    static constexpr const char* name    = "My Plugin";
    static constexpr const char* version = "1.0.0";

    // Asset enums
    enum class Images { Background, Icon };
    enum class Fonts  { Main };
    enum class Sounds { Click };

    // Config enums
    enum class Text     { WindowTitle };
    enum class Ints     { MaxPlayers };
    enum class Floats   { UIScale };
    enum class Booleans { ShowHUD };

    // Component flags and states
    enum class Flags { Selected, Highlighted };
    enum class State { Idle, Active, Paused };

private:
    inline static const auto _ = [](){
        set(Images::Background, "assets/bg.png");
        set(Images::Icon,       "assets/icon.png");
        set(Fonts::Main,        "assets/main.ttf");
        set(Sounds::Click,      "assets/click.wav");

        set(Text::WindowTitle, std::string("My App"));
        set(Ints::MaxPlayers,  4);
        set(Floats::UIScale,   1.0f);
        set(Booleans::ShowHUD, true);
        return 0;
    }();
};
```

> **The inline static initializer must be `private`.** It is an implementation detail that populates the framework's internal maps at program startup — before `main()`.

### Attaching a manifest to a component

```cpp
#include <Malena/Core/Component.h>
#include "MyManifest.h"

class MyWidget : public ml::ComponentWith<MyManifest>
{
    void initialization() override
    {
        // Enum aliases are in scope — no MyManifest:: prefix needed
        auto& tex   = Resources::get(Images::Background);
        auto& font  = Resources::get(Fonts::Main);
        auto& title = Resources::get(Text::WindowTitle);
        int   count = Resources::get(Ints::MaxPlayers);

        // Pointer form for SFML calls that take const sf::Texture*
        sprite.setTexture(&Resources::get(Images::Icon));

        // Flags and states wired automatically
        setState(State::Idle);
        enableFlag(Flags::Selected);
    }
};
```

### Enum types reference

| Enum name | Manager | Stored type |
|-----------|---------|-------------|
| `Images`   | `TextureManager` | `sf::Texture` |
| `Fonts`    | `FontManager`    | `sf::Font` |
| `Sounds`   | `SoundManager`   | `sf::SoundBuffer` |
| `Text`     | `ConfigManager`  | `std::string` |
| `Ints`     | `ConfigManager`  | `int` |
| `Floats`   | `ConfigManager`  | `float` |
| `Booleans` | `ConfigManager`  | `bool` |
| `Flags`    | `FlagManager`    | binary on/off |
| `State`    | `StateManager`   | exclusive state |

All enum types are optional — only declare the ones your component needs.

---

## Context — App-wide Resources

`ml::Context<Manifest>` combines `AssetsManager`, `ConfigManager`, and `StateManager` into one struct and exposes all manifest enum aliases. Use it to share a single manifest's resources globally without passing managers everywhere.

### Define the manifest

```cpp
// AppManifest.h
#pragma once
#include <Malena/Manifests/Manifest.h>

class AppManifest : public ml::Manifest
{
public:
    enum class Images { Splash, Logo };
    enum class Fonts  { UI };
    enum class Text   { AppTitle };

private:
    inline static const auto _ = [](){
        set(Images::Splash, "assets/splash.png");
        set(Images::Logo,   "assets/logo.png");
        set(Fonts::UI,      "assets/ui.ttf");
        set(Text::AppTitle, std::string("My App"));
        return 0;
    }();
};
```

### Create a global alias

```cpp
// AppContext.h
#pragma once
#include <Malena/Manifests/Context.h>
#include "AppManifest.h"

using App = ml::Context<AppManifest>;
```

### Use across your app

```cpp
#include "AppContext.h"

// Inside any class or function — no managers to pass around
auto& font  = App::AssetMgr::get(AppManifest::Fonts::UI);
auto& logo  = App::AssetMgr::get(AppManifest::Images::Logo);
auto& title = App::ConfigMgr::get(AppManifest::Text::AppTitle);
```

### Use inside Application

```cpp
#include <Malena/Engine/App/Application.h>
#include "AppContext.h"

class MyApp : public ml::Application
{
    ml::Text   titleText;
    ml::Sprite splashSprite;

public:
    MyApp() : ml::Application(sf::VideoMode({1280, 720}), "My App") {}

    void initialization() override
    {
        titleText.setFont(App::AssetMgr::get(AppManifest::Fonts::UI));
        titleText.setString(App::ConfigMgr::get(AppManifest::Text::AppTitle));
        titleText.setPosition({40, 24});

        splashSprite.setTexture(App::AssetMgr::get(AppManifest::Images::Splash));
        splashSprite.setPosition({0, 0});

        addComponent(titleText);
        addComponent(splashSprite);
    }

    void registerEvents() override {}
};

int main() { MyApp().run(); }
```

---

## Plugins

Malena's plugin system lets you build modular, dynamically-loaded extensions. A plugin subclasses `ml::PluginWith<MyManifest>` — the manifest must declare `name` and `version`. Use `ML_EXPORT` to register the plugin with the framework.

```cpp
// MyPlugin.h
#pragma once
#include <Malena/Engine/Plugins/Plugin.h>
#include "MyManifest.h"

class MyPlugin : public ml::PluginWith<MyManifest>
{
public:
    void onLoad()   override { /* called when the plugin is loaded */ }
    void onUnload() override { /* called when the plugin is unloaded */ }
};

ML_EXPORT(MyPlugin)
```

A plugin's manifest thumbnail is set automatically if `Images::THUMBNAIL` is declared:

```cpp
enum class Images { THUMBNAIL, Background };
// set(Images::THUMBNAIL, "assets/thumb.png");
```

---

## Best Practices

### Lifecycle
Create and configure components in `initialization()`. Wire all event callbacks in `registerEvents()`. Always call `addComponent(...)` for anything that should be updated and drawn.

### Events
Prefer the component helper callbacks (`onClick`, `onHover`, `onUpdate`) over polling. Use custom event channels for decoupled, cross-component communication.

### Flags
Most flags (`HOVERED`, `FOCUSED`, `CLICKED`) are managed automatically. Check them inside `onUpdate` for continuous per-frame behavior rather than re-subscribing.

### Layout
Use `setRightOf`, `setBelow`, `center`, `centerHorizonally`, and `centerText` instead of hardcoding positions. This makes layouts resilient to size changes.

### Manifests and Context
- Manifests **must** inherit `ml::Manifest`.
- Keep the `set(...)` calls in the `private:` inline static initializer.
- Create one `Context<YourManifest>` alias (e.g., `using App = ml::Context<AppManifest>`) and include it wherever you need global resources — this avoids passing manager references through constructors.

---

## Tutorials

Video tutorials are available on YouTube:

[![Watch the Malena Playlist](https://img.shields.io/badge/Watch%20Playlist-YouTube-red?logo=youtube)](https://www.youtube.com/playlist?list=PLAK6gn2iUWAaOaDcVA5a3A7SFMW8ugor0)

**https://www.youtube.com/playlist?list=PLAK6gn2iUWAaOaDcVA5a3A7SFMW8ugor0**

Full documentation is available at **[malena.app](https://malena.app)**.

---

## License

MIT. See [`LICENSE.md`](LICENSE.md).