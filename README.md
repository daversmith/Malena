# Malena

> A lightweight, event‑driven C++ UI/game framework built on SFML

Malena makes it easy to create interactive applications in C++ by providing:

- A zero‑boilerplate **Application** base class
- Built‑in **UI components** (buttons, text inputs, shapes…)
- A flexible **publish/subscribe** event system
- Automatic **update** + **render** loops
- Modular **Managers** (Window, Font, UI, etc.)
- Useful **Utilities** (tweening, text wrapping, helpers)
- MVC & EDA design patterns

---

## 📦 Requirements

-   C++17 compatible compiler
-   **CMake 3.14** or newer
-   **SFML 3.0** (Can be installed system-wide or fetched automatically via CMake)

---

## 🚀 Building the Examples

You can build the included examples using CMake:

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/daversmith/Malena.git
    cd Malena
    ```
2.  **Configure with CMake:** (Enable example building)
    ```bash
    # out-of-source builds are recommended!
    cmake -B build -S . -DMALENA_BUILD_EXAMPLES=ON
    ```
    * By default, CMake will fetch SFML 3.0 automatically if it's not found or if `MALENA_USE_SYSTEM_SFML` is OFF (default).
    * To use a pre-installed SFML 3.0, you can configure like this:
        ```bash
        # Ensure CMake can find your SFML installation (you might need to set CMAKE_PREFIX_PATH or SFML_DIR)
        cmake -B build -S . -DMALENA_BUILD_EXAMPLES=ON -DMALENA_USE_SYSTEM_SFML=ON
        ```

3.  **Build:**
    ```bash
    cmake --build build
    # For multi-config generators (like Visual Studio or Xcode), specify the configuration:
    # cmake --build build --config Release
    ```
4.  **Run an example:** (Executable location depends on generator and platform)
    ```bash
    # Example for Makefiles/Ninja on Linux/macOS:
    ./build/examples/BasicExample/BasicExample

    # Example for Visual Studio on Windows (assuming Release config):
    .\build\examples\BasicExample\Release\BasicExample.exe
    ```

---

## 🛠️ Using Malena in Your CMake Project (FetchContent)

You can easily integrate Malena into your own CMake project using `WorkspaceContent`. This allows CMake to automatically download and configure Malena as part of your build process.

1.  **Modify your `CMakeLists.txt`:**

    ```cmake
    cmake_minimum_required(VERSION 3.14)
    project(CS8)

    include(FetchContent)
    FetchContent_Declare(
        malena
        GIT_REPOSITORY https://github.com/daversmith/Malena.git
        GIT_TAG v0.2.4
    )
    FetchContent_MakeAvailable(malena)

    add_executable(${PROJECT_NAME}
        main.cpp
        TextureSlicerDemo.cpp
        TextureSlicerDemo.h
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE Malena::Malena)
    ```

2.  **Configure and Build Your Project:**
    When you run CMake for your project, it will handle Malena:
    ```bash
    # Configure (from your project's root)
    cmake -B build -S .

    # Build
    cmake --build build
    ```

---

## 📁 Directory Layout

```
Malena/
├── examples/         ← Example applications using Malena (e.g., BasicExample/)
├── include/Malena/   ← Public headers (.h/.hpp) for the library
│   ├── Application/  ← Application framework and lifecycle management classes
│   ├── Managers/     ← Subsystem managers (UI, components, events, font resources)
│   ├── Interfaces/   ← Abstract interfaces and component base classes
│   ├── Graphics/     ← UI components, shapes, text rendering, visual elements
│   ├── Controllers/  ← Input handlers and application control logic
│   ├── Utilities/    ← Helper functions, text manipulation, mouse interaction utilities
│   └── Traits/       ← Reusable behaviors (Stateful, Positionable, Messenger) for composition
├── src/           
│   ├── Application/
│   ├── Managers/
│   ├── ... (etc.)
├── cmake/            ← Helper CMake scripts (e.g., MalenaConfig.cmake.in)
├── .github/          ← GitHub Actions workflows (if used)
├── CMakeLists.txt    ← Root CMake build script for Malena library
└── README.md
```

---

## 📘 Usage

### Create your own app

```cpp
class MyApp : public ml::Application {
public:
    MyApp() : Application({800,600}, "My Malena App", *this) {}
    void initialization() override { /* add UI components here */ }
    void registerEvents() override { /* subscribe to events here */ }
};

int main() {
    MyApp app;
    app.run();
}
```

⚠️ **Important:** All UI components must be registered with the application using `addComponent(...)` inside the `initialization()` method. Otherwise, they will not be rendered or updated.

---

## 📋 Event System

| Method | What it does |
|--------|--------------|
| `subscribe(event, component, callback)` | Listen for `event` |
| `unsubscribe(event, component)` | Remove single handler |
| `unsubscribeAll(component)` | Remove all of a component’s handlers |
| `clearAllEvents()` | Remove **every** subscription |
| `publish(event, filter)` (alias of `fire`) | Trigger callbacks |

### Subscribing to Custom Events

Any component (or system) can subscribe to and publish custom-named events using `subscribe` and `publish`. Here's how to define your own event channel:

```cpp
// Subscribe to a custom event named "enemyDefeated"
myComponent.subscribe("enemyDefeated", [](){
    std::cout << "Enemy defeated! Bonus awarded.\n";
});

// Trigger the event manually from another component
triggerButton.onClick([&](){
    triggerButton.publish("enemyDefeated");
});
```

You can use filters to control which components respond to published events:

```cpp
EventsManager::fire("enemyDefeated", [](UIComponent& c){
    return c.hasTag("Rewardable");
});
```

---

## 🖱 UI Components

All inherit from `ml::UIComponent`:

| Component           | Purpose |
|---------------------|---------|
| `RectangleButton`   | Clickable rectangle with text |
| `TextBox`           | Multi‑line text display |
| `Typer`             | Text input field |
| `Circle`            | Drawable circle shape |
| `Grid`              | Grid layout or game board utility |
| `Sprite`            | SFML `sf::Sprite` wrapper with UI integration |

These components expose intuitive event hooks:

```cpp
box.onHover([](){ box.setFillColor(sf::Color::Yellow); });
box.onClick([](){ std::cout << "Clicked!\n"; });
```

---

## 🔧 Managers

- **WindowManager**: Single global SFML window
- **UIManager**: Draws & updates components
- **FontManager**: Central font loading
- **ComponentsManager**: Global component registry

---

## ⚙️ Utilities & Traits

- **Tween**: Smooth interpolation (move, fade, etc.)
- **TextManipulators**: Word wrapping and trimming helpers
- **Positionable**: Anchoring, sizing, alignment
- **Stateful**: Handle component states like hover, focus, active
- **Messenger**: Trigger events or communicate via string‑based keys

---

## 📖 BasicExample Explained

The `BasicExample.cpp` demonstrates how to:

- Add components using `addComponent()`
- Respond to hover, click, and keypress events
- Dynamically update and style UI elements
- Use custom events via `subscribe()` and `publish()`

### Setup
```cpp
addComponent(box1);         // Display area
addComponent(typer);        // Typing field
addComponent(subscribeBtn); // Button to subscribe to custom event
addComponent(myEventButton);// Button to trigger the custom event
```

### Focus Handling + Live Text Update
```cpp
box1.onFocus([&](){
    box1.setString("Focused");
});

typer.onKeypress([&](){
    if(box1.checkState(ml::Stateful::FOCUSED)) {
        box1.setString(typer.getString());
    }
});
```

### Custom Event Subscription
```cpp
subscribeBtn.onClick([&](){
    box1.subscribe("myEvent", [](){
        std::cout << "myEvent triggered!\n";
    });
});
```

### Custom Event Firing
```cpp
myEventButton.onClick([&](){
    myEventButton.publish("myEvent");
});
```

### Extra Interactivity
```cpp
box1.onHover([&](){
    box1.setFillColor(sf::Color::Green);
});

box1.onClick([&](){
    box1.tweenPosition({300, 300}, 0.3f);
});
```

Each component in the example illustrates a piece of Malena’s event‑driven architecture. Together, they show how to:

- Create interactive widgets
- Attach dynamic behavior
- Use event channels for cross‑component messaging

---

## 📺 Demo Video

If you’d like to see Malena in action, check out this short walkthrough:

[![Watch the demo](https://img.youtube.com/vi/-LSqvIpHAKY/0.jpg)](https://youtu.be/-LSqvIpHAKY)



---
## ❤️ Contributing 

Malena is MIT‑style friendly. Fork, improve, submit PRs — and happy coding!
