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
│   ├── Graphics/               ← Buttons, TextBox, Shapes, Typer
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

---

## 📋 Event System

| Method | What it does |
|--------|--------------|
| `subscribe(event, component, callback)` | Listen for `event` |
| `unsubscribe(event, component)` | Remove single handler |
| `unsubscribeAll(component)` | Remove all of a component’s handlers |
| `clearAllEvents()` | Remove **every** subscription |
| `publish(event, filter)` (alias of `fire`) | Trigger callbacks |

```cpp
button.onClick([&](){ /* UIComponent publishes */ });
box.subscribe("myEvent", [](){ /* handle */ });
button.publish("myEvent", [](ml::UIComponent& c){ return true; });
ml::EventsManager::clearAllEvents();
```

---

## 🖱 UI Components

All inherit from `ml::UIComponent`:

| Component         | Purpose |
|-------------------|---------|
| `RectangleButton` | Clickable rectangle with text |
| `TextBox`         | Multi‑line text display |
| `Typer`           | Text input field |
| `Shape`           | Base for circles, grids, sprites… |

They expose easy-to-use event hooks:

```cpp
box.onHover([](){ box.setFillColor(sf::Color::Yellow); });
box.onClick([](){ std::cout<<"Clicked!\n"; });
```

---

## 🔧 Managers

- **WindowManager**: Single global SFML window
- **UIManager**: Draws & updates components
- **FontManager**: Central font loading
- **ComponentsManager**: Global component registry

---

## ⚙️ Utilities & Traits

- **Tween**: Smooth interpolation
- **TextManipulators**: Word wrap, trimming
- **Positionable**: Easy relative positioning
- **Stateful**: Manage hover/focus/disabled states

---

## 📖 BasicExample Snippet

```cpp
// In registerEvents()
box1.onFocus([&](){ box1.setString("Focused"); });
typer.onKeypress([&](){
    if(box1.checkState(ml::Stateful::FOCUSED))
        box1.setString(typer.getString());
});
subscribeBtn.onClick([&](){ box1.subscribe("myEvent", [](){ /*…*/ }); });
myEventButton.onClick([&](){
    myEventButton.publish("myEvent");
});
```

---

## ❤️ Contributing

Malena is MIT‑style friendly. Fork, improve, submit PRs — and happy coding!
