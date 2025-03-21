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

- C++17 or newer
- SFML ≥ 2.5
- Your favorite build system (Makefile/CMake/etc.)

---

## 🚀 Quickstart (Basic Example)

Clone the repo, then compile+link against SFML:

```bash
g++ -std=c++17 \
    -I src -I examples/BasicExample \
    src/main.cpp \
    src/**/*.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o BasicExample
./BasicExample
```

---

## 📁 Directory Layout

```
Malena/
├── examples/BasicExample/      ← Simple “Hello world” demo
├── src/
│   ├── Application/            ← ml::Application base class
│   ├── Managers/               ← Window, UI, Event, Font, Components
│   ├── Interfaces/             ← UIComponent, Updateable
│   ├── Graphics/               ← RectangleButton, TextBox, Typer, Grid, Circle, Sprite
│   ├── Controllers/            ← UIController
│   ├── Utilities/              ← Tween, TextManipulators, MouseEvents
│   └── Traits/                 ← Positionable, Stateful, Messenger
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

## ❤️ Contributing

Malena is MIT‑style friendly. Fork, improve, submit PRs — and happy coding!
