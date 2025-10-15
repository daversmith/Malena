# Malena Framework – README

- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [Application](#application)
- [UI Components (Complete)](#ui-components-complete)
- [Events and Messaging](#events-and-messaging)
- [Flags and State](#flags-and-state)
- [Positioning and Layout helpers](#positioning-and-layout-helpers)
- [Manifest System (Accurate Pattern)](#manifest-system-accurate-pattern)
- [Using a Single Context Across Your App](#using-a-single-context-across-your-app)
- [Best Practices](#best-practices)
- [License](#license)

---

## Overview

**In this section:**  
[What Malena provides](#what-malena-provides) · [Where things live](#where-things-live)

### What Malena provides

Malena is a lightweight, event-driven C++ framework on top of SFML. It provides:

- An `Application` shell that opens a window and runs your loop
- A publish/subscribe event system with friendly component helpers
- A focused set of UI **graphics components** that wrap SFML drawables
- Positioning helpers for simple relative layout
- A *Manifest* pattern for type-safe assets and config
- A *Context* façade to use one manifest globally across your app

### Where things live

- Core types: `include/Malena/*`
- Example app: `examples/BasicExample`

---

## Requirements

**In this section:**  
[C++ standard](#c-standard) · [CMake](#cmake) · [SFML](#sfml)

### C++ standard
C++17 or newer

### CMake
CMake 3.14+

### SFML
SFML 3 (Malena wraps SFML drawables; your app links SFML as usual)

---

## Installation

**In this section:**  
[Clone](#clone) · [Build the bundled example](#build-the-bundled-example) · [Add as subdirectory (your app)](#add-as-subdirectory-your-app) · [FetchContent (alternative)](#fetchcontent-alternative)

> This mirrors the style used on the GitHub page: clone, optionally build examples, or consume via CMake with a simple link target.

### Clone

```bash
git clone https://github.com/daversmith/Malena.git
```

### Build the bundled example

```bash
cd Malena
cmake -B build -S . -DMALENA_BUILD_EXAMPLES=ON
cmake --build build
# run the example (path depends on your generator)
./build/examples/BasicExample/BasicExample
```

### Add as subdirectory (your app)

```cmake
# CMakeLists.txt of your app
cmake_minimum_required(VERSION 3.14)
project(MyMalenaApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/Malena)  # where you cloned Malena

add_executable(${PROJECT_NAME}
  src/main.cpp
)

# Malena exports an interface target; link it
target_link_libraries(${PROJECT_NAME} PRIVATE Malena::Malena)
```

### FetchContent (alternative)

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyMalenaProject)

include(FetchContent)
FetchContent_Declare(
        malena
        GIT_REPOSITORY https://github.com/daversmith/Malena.git
        GIT_TAG "v0.3.4"
)
FetchContent_MakeAvailable(malena)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Malena::Malena)
```

---

## Quick Start

**In this section:**  
[Minimal app](#minimal-app) · [Where to put code](#where-to-put-code)

### Minimal app

```cpp
#include <Malena/Application/Application.h>
#include <Malena/Graphics/RectangleButton.h>

class MyApp : public ml::Application {
  ml::RectangleButton button;

public:
  MyApp()
  : ml::Application(sf::VideoMode({800, 480}, 32), "My App", *this) {}

  void initialization() override {
    button.setSize({240, 64});
    button.setPosition({40, 40});
    button.setString("Click me");
    addComponent(button);
  }

  void registerEvents() override {
    button.onClick([this]{ button.setString("Clicked!"); });
  }
};

int main() { MyApp().run(); }
```

### Where to put code

- Create and register components in `initialization()`
- Subscribe to events in `registerEvents()`

---

## Core Concepts

**In this section:**  
[Application shell](#application-shell) · [UI components](#ui-components) · [Events](#events) · [Flags](#flags) · [Positioning](#positioning)

### Application shell
`ml::Application` runs the loop and mixes in `UIManager` (loop, draw, input firing) and `UIController` (your logic).

### UI components
Wrappers over SFML drawables with traits: `Messenger` (events), `Flaggable` (flags), `Positionable` (layout), `Customizable` (manifests).

### Events
String-keyed; built-ins include `click`, `hover`, `unhover`, `focus`, `blur`, `keypress`, `textentered`, `mousemoved`, and `update`. You can publish your own.

### Flags
State bits such as `HOVERED`, `FOCUSED`, `ENABLED`, `HIDDEN`. Some are maintained automatically.

### Positioning
Helpers like `right`, `left`, `top`, `bottom`, `center`, `xCenter`, `yCenter` and `centerText` simplify layout.

---

## Application

**In this section:**  
[Constructors](#constructors) · [Lifecycle](#lifecycle)

### Constructors

Common forms include:

- `Application(const sf::VideoMode&, const std::string& title, UIController& appLogic, sf::RenderWindow& window = WindowManager::getWindow())`
- `Application(unsigned int w, unsigned int h, unsigned int bitDepth, const std::string& title)`
- `Application(const sf::VideoMode&, const std::string& title)`

Pass `*this` as the `UIController` when deriving.

### Lifecycle

- `initialization()` — create components, `addComponent(...)`
- `registerEvents()` — attach `onClick`, `onHover`, `onUpdate`, custom `subscribe`
- `run()` — poll events → fire → update → draw

---

## UI Components (Complete)

**In this section:**  
[List](#list) · [Examples](#examples)

### List

Graphics components in `include/Malena/Graphics`:

- **Button.h** – generic button template used by specific button shapes
- **Circle.h** – wraps `sf::CircleShape`
- **CircleButton.h** – circular button with centered text
- **Convex.h** – wraps `sf::ConvexShape`
- **ConvexButton.h** – convex-shape button with centered text
- **Cursor.h** – caret for text input
- **Grid.h** – grid helper for cell positioning/placement
- **Rectangle.h** – wraps `sf::RectangleShape`
- **RectangleButton.h** – rectangular button with centered text
- **Shape.h** – base helper to wrap an SFML shape drawable
- **Sprite.h** – wraps `sf::Sprite`
- **Text.h** – wraps `sf::Text`, with optional word-wrap controls (e.g., `setWordWrap`, `setMaxWidth`)
- **TextBox.h** – framed text display area
- **TextInput.h** – composite input field using `Typer` and `Cursor`
- **Typer.h** – text entry logic (focus, textentered, keypress)
- **VertexArray.h** – wraps `sf::VertexArray`

### Examples

**RectangleButton**

```cpp
#include <Malena/Graphics/RectangleButton.h>

ml::RectangleButton submit;
submit.setSize({200, 50});
submit.setPosition({40, 40});
submit.setString("Submit");
addComponent(submit);

submit.onHover([&]{ submit.setFillColor(sf::Color(180,220,255)); });
submit.onUnhover([&]{ submit.setFillColor(sf::Color(100,160,255)); });
submit.onClick([&]{ /* ... */ });
```

**Text + TextInput**

```cpp
#include <Malena/Graphics/Text.h>
#include <Malena/Graphics/TextInput.h>

ml::Text label;
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
#include <Malena/Graphics/Sprite.h>

ml::Sprite icon;
icon.setTexture(/* sf::Texture */);
icon.setPosition({320, 60});
addComponent(icon);

icon.onClick([&]{ /* ... */ });
```

**Grid**

```cpp
#include <Malena/Graphics/Grid.h>

ml::Grid grid(3, 3);
grid.setPosition({40, 220});
grid.setCellSize({72, 72});
grid.setSpacing(8, 8);
addComponent(grid);
```

---

## Events and Messaging

**In this section:**  
[Built-in events](#built-in-events) · [Component helpers](#component-helpers) · [Custom events](#custom-events)

### Built-in events
`click`, `hover`, `unhover`, `focus`, `blur`, `keypress`, `textentered`, `mousemoved`, `update`

### Component helpers

```cpp
button.onClick([]{});
button.onHover([]{});
button.onUnhover([]{});
button.onFocus([]{});
button.onBlur([]{});
button.onKeypress([](const std::optional<sf::Event>&){});
button.onMouseMoved([](const std::optional<sf::Event>&){});
button.onTextEntered([](const std::optional<sf::Event>&){});
button.onUpdate([]{});
```

### Custom events

```cpp
box.subscribe("MyEvent", []{ /* ... */ });
ml::Messenger::publish("MyEvent");
box.unsubscribe("MyEvent");
box.unsubscribeAll();

// publish to others, optionally filter
ml::Messenger::publish("MyEvent", [](ml::UIComponent& c){
  return !c.checkFlag(ml::Flag::HIDDEN);
});
```

---

## Flags and State

**In this section:**  
[Common flags](#common-flags) · [Operations](#operations)

### Common flags
`HOVERED`, `CLICKED`, `FOCUSED`, `ENABLED`, `HIDDEN`, `BLINKING`

### Operations

```cpp
if (btn.checkFlag(ml::Flag::HOVERED)) { /* … */ }
btn.enableFlag(ml::Flag::ENABLED);
btn.disableFlag(ml::Flag::HIDDEN);
btn.toggleFlag(ml::Flag::FOCUSED);
btn.setFlag(ml::Flag::ENABLED, true);
```

---

## Positioning and Layout helpers

**In this section:**  
[Relative placement](#relative-placement) · [Centering](#centering) · [Motion helpers](#motion-helpers)

### Relative placement
`right(of, spacing)`, `left(of, spacing)`, `top(of, spacing)`, `bottom(of, spacing)`

### Centering
`center(of)`, `xCenter(of)`, `yCenter(of)`, `centerText(sf::Text&)`

### Motion helpers
`moveTo`, `moveDistance`, `calcVelocity` (simple queue-based tweening helpers)

---

## Manifest System (Accurate Pattern)

**In this section:**  
[Manifest class](#manifest-class) · [Registering values](#registering-values) · [Access via managers](#access-via-managers)

> **Important:** Your manifests should **inherit** `ml::Manifest`.

### Manifest class

```cpp
// AssetsManifest.h  (aligned with your file)
#include <Malena/Manifests/Manifest.h>

class AssetsManifest : public ml::Manifest {
public:
  enum class Fonts  { OPEN_SANS };
  enum class Images { FLYER, SCREENSHOT };
  enum class Text   { PLEASE };

  struct RectangleConfig { enum class Flags { STOP, EAT }; };

private:
  inline static const auto _ = [](){
    set(
      Images::FLYER,      "assets/images/image002.jpg",
      Images::SCREENSHOT, "assets/fonts/thumbnail_Screenshot_20210605-131717_Messages.jpg"
    );
    set(Fonts::OPEN_SANS, "assets/fonts/OpenSans-Light.ttf");
    set(Text::PLEASE,     std::string("Please Work!!!"));
    return 0;
  }();
};
```

### Registering values

- Call `set(...)` inside a static initializer (or constructor) to associate enum IDs with file paths or config values.
- You can create small specialized manifests as well (e.g., `BasicImageManifest`, `BasicManifest` with a single enum).

### Access via managers

```cpp
#include <Malena/Managers/TextureManager.h>
#include <Malena/Managers/FontManager.h>
#include <Malena/Managers/SoundManager.h>
#include <Malena/Managers/ConfigManager.h>

using Tex  = ml::TextureManager<AssetsManifest>;
using Font = ml::FontManager<AssetsManifest>;
using Cfg  = ml::ConfigManager<AssetsManifest>;

const sf::Texture& flyer = Tex::get(AssetsManifest::Images::FLYER);
const sf::Font&    ui    = Font::get(AssetsManifest::Fonts::OPEN_SANS);
const std::string& please= Cfg::get<AssetsManifest>(AssetsManifest::Text::PLEASE);
```

---

## Using a Single Context Across Your App

**In this section:**  
[Define one manifest](#define-one-manifest) · [Create the Context alias](#create-the-context-alias) · [Use across UI](#use-across-ui) · [Use inside Application](#use-inside-application)

The **Context** façade binds all managers to your manifest and exposes typed access in one place.

### Define one manifest
Use `AssetsManifest` above (inherits `ml::Manifest`).

### Create the Context alias

```cpp
// AppContext.h
#pragma once
#include <Malena/Manifests/Context.h>
#include "AssetsManifest.h"

using App = ml::Context<AssetsManifest>; // One context for the whole app
```

### Use across UI

```cpp
#include "AppContext.h"
#include <Malena/Graphics/Text.h>
#include <Malena/Graphics/Sprite.h>

class MyPanel {
  ml::Text   title;
  ml::Sprite image;

public:
  void build() {
    title.setFont(App::Assets::get(AssetsManifest::Fonts::OPEN_SANS));
    title.setString(App::Config::get(AssetsManifest::Text::PLEASE));
    image.setTexture(App::Assets::get(AssetsManifest::Images::FLYER));
  }
};
```

### Use inside Application

```cpp
#include <Malena/Application/Application.h>
#include "AppContext.h"
#include <Malena/Graphics/RectangleButton.h>

class ContextDemo : public ml::Application {
  ml::RectangleButton start;
  ml::Text            title;
  ml::Sprite          flyer;

public:
  ContextDemo()
  : ml::Application(720, 420, 32, "Context Demo", *this),
    flyer(App::Assets::get(AssetsManifest::Images::FLYER)) {}

  void initialization() override {
    title.setFont(App::Assets::get(AssetsManifest::Fonts::OPEN_SANS));
    title.setString("Welcome");
    title.setPosition({40, 24});

    start.setSize({220, 56});
    start.setString("Start");
    start.setPosition({40, 80});

    addComponent(title);
    addComponent(start);
    addComponent(flyer);
  }

  void registerEvents() override {
    start.onClick([this]{
      // Example: swap texture at runtime using the same context
      flyer.setTexture(App::Assets::get(AssetsManifest::Images::SCREENSHOT));
    });
  }
};

int main() { ContextDemo().run(); }
```

---

## Best Practices

**In this section:**  
[Lifecycle](#lifecycle-1) · [Events](#events-1) · [Flags](#flags-1) · [Layout](#layout) · [Manifests & Context](#manifests--context)

### Lifecycle
Create components in `initialization()`, wire events in `registerEvents()`, and always `addComponent(...)` for things you want updated/drawn.

### Events
Prefer component helpers (`onClick`, `onHover`, `onUpdate`) over polling; use custom channels for decoupled logic.

### Flags
Many flags are updated automatically (e.g., `HOVERED`, `FOCUSED`). Check them in `onUpdate` for continuous behaviors.

### Layout
Use `bottom`, `right`, `center`, `xCenter`, `yCenter`, and `centerText` to simplify placement.

### Manifests & Context
- **Manifests must inherit `ml::Manifest`.**
- Register filepaths/config via `set(...)`.
- Use a single `Context<YourManifest>` alias (`App`) across your app for clean, typed access to assets and config.

---
# Malena Tutorials Playlist

This page embeds the Malena tutorials playlist. If your Markdown renderer supports HTML, the playlist will display below. Otherwise, use the direct link.

**Watch on YouTube:** https://www.youtube.com/playlist?list=PLAK6gn2iUWAaOaDcVA5a3A7SFMW8ugor0

<!-- HTML embed (works in many Markdown renderers that allow HTML, such as Canvas pages) -->
<div align="center">
  <iframe
    width="560"
    height="315"
    src="https://www.youtube.com/embed/videoseries?list=PLAK6gn2iUWAaOaDcVA5a3A7SFMW8ugor0"
    title="YouTube playlist player"
    frameborder="0"
    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
    allowfullscreen>
  </iframe>
</div>

## License

**In this section:**  
[MIT License](#mit-license)

### MIT License
MIT. See `LICENSE.md`.
