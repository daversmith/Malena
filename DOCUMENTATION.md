# Game Development with Your SFML Framework

## Introduction
This documentation provides an overview of how to use the game development framework built with SFML. The framework follows an object-oriented and event-driven approach, making it easy to create interactive applications.

## Key Components

### 1. `Application`
- The core class that initializes and runs the application.
- Inherits from `UIManager` and `AppLogic`.
- Handles the event loop and component management.

### 2. `AppLogic`
- A base class for handling game logic.
- Requires implementation of:
    - `initialization()`: Initializes game objects.
    - `registerEvents()`: Registers event listeners.
    - `update()`: Updates game logic every frame.

### 3. `UIManager`
- Manages the game window and rendering.
- Calls `AppLogic` methods for logic updates.
- Handles events and interactions.

### 4. `ComponentsManager`
- Manages multiple `Component` objects.
- Stores dynamically created components and updates them.

### 5. `Component`
- Base class for all drawable and interactive objects.
- Inherits from `sf::Drawable` and `MessageManager`.
- Requires implementation of:
    - `update()`: Updates the component state.
    - `eventHandler()`: Handles events.
    - `getGlobalBounds()`: Returns the object's boundaries for interactions.

### 6. `WindowManager`
- Provides access to a single SFML `sf::RenderWindow`.
- Used for rendering components.

### 7. `EventsManager`
- Manages and dispatches game events.
- Supports event subscriptions and message passing.

### 8. `StateManager`
- Maintains state flags such as `HOVERED` and `CLICKED`.
- Used for interactive components.

### 9. `MouseEvents`
- Provides utility functions to check if an object is hovered or clicked.

### 10. `MessageManager`
- Allows components to publish and subscribe to events.

---

## Getting Started: Creating a Simple Game

### Step 1: Define Your Game Class
Create a game class that inherits from `Application` and implements `AppLogic`.

```tsx
#include "Application.h"
#include "Component.h"
#include "StateManager.h"

class RectangleGame : public Application {
    sf::RectangleShape rectangle;

public:
    RectangleGame() : Application(sf::VideoMode(800, 600), "Rectangle Game", *this) {
        rectangle.setSize({100, 100});
        rectangle.setPosition({200, 200});
        rectangle.setFillColor(sf::Color::Blue);
    }

    void initialization() override {
        addComponent(rectangle);
    }

    void registerEvents() override {
        rectangle.onClick([this]() {
            rectangle.setFillColor(sf::Color::Red);
        });
    }

    void update() override {}
};
```

### Step 2: Run the Game
```tsx
int main() {
    RectangleGame game;
    game.run();
    return 0;
}
```

## Event Handling
Events can be registered and fired using `MessageManager` and `EventsManager`.

```tsx
someComponent.onClick([]() {
    std::cout << "Component clicked!" << std::endl;
});
```

## Summary
This framework provides an easy-to-use structure for building SFML games. By leveraging `Application`, `Component`, `UIManager`, and event-driven mechanics, you can quickly develop interactive applications.

For more complex applications, extend `Component` to create custom UI elements, use `StateManager` for object states, and integrate `EventsManager` for game-wide event handling.

