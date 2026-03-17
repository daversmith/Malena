# Getting Started with Malena

Malena is a C++ framework built on top of SFML that provides an event-driven architecture, trait-based composition, and reusable graphical components.

---

## Basic Application

A Malena application derives from `ml::Application` and overrides two virtual methods:

- `initialization()` — runs once at startup; create and register components here
- `registerEvents()` — runs after initialization; subscribe to events here

```cpp
#include <Malena/Engine/App/Application.h>

class MyApp : public ml::Application
{
public:
    MyApp() : Application(1280, 720, 32, "My Application") {}

    void initialization() override
    {
        _box.setSize({200.f, 100.f});
        _box.setPosition({100.f, 100.f});
        addComponent(_box);
    }

    void registerEvents() override
    {
        _box.onClick([] {
            // respond to click
        });

        _box.onHover([this] {
            _box.setFillColor(sf::Color::Yellow);
        });

        _box.onUnhover([this] {
            _box.setFillColor(sf::Color::White);
        });
    }

private:
    ml::Rectangle _box;
};

int main()
{
    MyApp app;
    app.run();
}
```

---

## Adding Components

Components are registered with `addComponent()` and are then automatically updated and drawn each frame by `ComponentsManager`.

```cpp
void initialization() override
{
    _label.setString("Hello, Malena!");
    _label.setPosition({50.f, 50.f});
    addComponent(_label);

    _button.setSize({120.f, 40.f});
    _button.setPosition({50.f, 120.f});
    addComponent(_button);
}
```

**Important:** Do not copy or move components after they have been registered with a manager. If you need a collection of components (e.g., a grid of tiles), use `std::vector<std::unique_ptr<MyComponent>>`.

---

## Event Subscriptions

Subscriptions are set up in `registerEvents()` using the `Messenger` trait methods available on any `UIComponent`:

```cpp
void registerEvents() override
{
    _button.onClick([this] {
        _label.setString("Clicked!");
    });

    _button.onUpdate([this](sf::Event) {
        // runs every frame
    });
}
```

Subscriptions are automatically cleaned up when a component is destroyed — no manual unsubscription needed in most cases.

---

## Manifests

A manifest is a nested struct that declares the resources, flags, and states a class uses. Combine it with the `With<Manifest>` template to wire everything together automatically.

### Minimal manifest with a texture

```cpp
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Sprite.h>

struct PlayerManifest
{
    struct Textures : ml::TextureList
    {
        TEXTURE(Idle,   "assets/player_idle.png")
        TEXTURE(Run,    "assets/player_run.png")
    };
};

class Player : public ml::SpriteWith<PlayerManifest>
{
public:
    Player()
    {
        setTexture(getTexture(PlayerManifest::Textures::Idle));
    }
};
```

### Manifest with flags

```cpp
struct WidgetManifest
{
    enum class Flags { Selected, Disabled };
};

class Widget : public ml::UIComponentWith<WidgetManifest>
{
public:
    void select()
    {
        setFlag(WidgetManifest::Flags::Selected);
    }

    bool isSelected() const
    {
        return checkFlag(WidgetManifest::Flags::Selected);
    }
};
```

### AssetsManager — for full media support

Use `AssetsManager` when a class needs textures, fonts, and sounds together:

```cpp
struct GameManifest
{
    struct Textures : ml::TextureList
    {
        TEXTURE(Background, "assets/bg.png")
    };
    struct Fonts : ml::FontList
    {
        FONT(Main, "assets/main.ttf")
    };
    struct Sounds : ml::SoundList
    {
        SOUND(Jump, "assets/jump.wav")
    };
};

// AssetsManager<GameManifest> gives access to all three media types
class GameScreen : public ml::UIComponentWith<GameManifest>
{
    void initialization() override
    {
        _bg.setTexture(getTexture(GameManifest::Textures::Background));
        _label.setFont(getFont(GameManifest::Fonts::Main));
        _jump.setBuffer(getSound(GameManifest::Sounds::Jump));
    }
};
```

### Context — assets + configuration

Use `Context` when you also need key-value configuration alongside media:

```cpp
struct AppManifest
{
    struct Config : ml::ConfigList
    {
        CONFIG(WindowTitle, "My App")
        CONFIG(MaxPlayers,  "4")
    };
    struct Textures : ml::TextureList
    {
        TEXTURE(Logo, "assets/logo.png")
    };
};

// Context<AppManifest> wires both AssetsManager and ConfigManager
class MyApp : public ml::Application
{
    void initialization() override
    {
        auto title = getConfig(AppManifest::Config::WindowTitle);
        setTitle(title);
    }
};
```

**When to use which:**

| You need | Use |
|---|---|
| Only one asset type | Individual manager (e.g., `TextureManager`) |
| Multiple media types | `AssetsManager` |
| Media + configuration | `Context` |

---

## Plugins

Plugins are dynamically loaded modules. The `PluginWith<Manifest>` base and the `REGISTER_PLUGIN` macro are all you need to create one.

```cpp
// MyPlugin.h
#include <Malena/Engine/Plugins/PluginWith.h>

class MyPlugin : public ml::PluginWith<MyPlugin::Manifest>
{
public:
    struct Manifest { /* resources, flags, etc. */ };

    void onLoad() override
    {
        // initialize plugin resources
    }

    void onUnload() override
    {
        // cleanup (called before destruction)
    }
};
```

```cpp
// MyPlugin.cpp
#include "MyPlugin.h"
#include <Malena/Engine/Plugins/RegisterPlugin.h>

REGISTER_PLUGIN(MyPlugin)
```

Load and query from the host application:

```cpp
pluginManager.loadPlugin("path/to/MyPlugin.dylib");

if (auto* p = pluginManager.getPlugin<MyPlugin>()) {
    // use p
}

// Query optional interface safely across dylib boundary
if (auto* drawable = plugin->getIf<ml::UIComponent>()) {
    addComponent(*drawable);
}
```

---

## Typed Messages

Use `sendMessage` and `onMessage` for typed communication between plugins or framework objects (keyed by a user-defined enum):

```cpp
enum class GameEvent { Start, Stop, ScoreChanged };

// Sender
sendMessage<int>(GameEvent::ScoreChanged, 42);

// Receiver — set up in registerEvents() or onLoad()
onMessage<int>(GameEvent::ScoreChanged, [](const int& score) {
    // update display
});
```

---

## Common Patterns

### Carousel

```cpp
ml::Carousel carousel;
carousel.setItemSize({200.f, 150.f});
carousel.setPosition({100.f, 200.f});
addComponent(carousel);

// Add items
carousel.addItem(mySprite);
carousel.addItem(anotherSprite);
```

### Grid layout

```cpp
ml::Grid grid(4, 3); // 4 columns, 3 rows
grid.setPosition({50.f, 50.f});
grid.setCellSize({100.f, 100.f});
addComponent(grid);
```

### TextInput

```cpp
ml::TextInput input;
input.setSize({300.f, 40.f});
input.setPosition({100.f, 300.f});
input.onUpdate([this](sf::Event e) {
    std::string text = input.getString();
});
addComponent(input);
```

---

## Next Steps

- **MALENA_CONCEPTS.md** — key framework concepts explained in depth
- **MALENA_ARCHITECTURE.md** — full subsystem breakdown, execution model, and architectural roles