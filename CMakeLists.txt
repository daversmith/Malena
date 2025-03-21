cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp
        src/Utilities/MouseEvents.cpp
        src/Utilities/MouseEvents.h
        src/Utilities/MouseEvents_nontemplate.cpp
        src/Utilities/MouseEvents_nontemplate.cpp
        src/Traits/Stateful.cpp
        src/Traits/Stateful.h
        src/Graphics/Rectangle.h
        src/Managers/UIManager.cpp
        src/Managers/UIManager.h
        src/Interfaces/UIComponent.cpp
        src/Interfaces/UIComponent.h
        src/Traits/Messenger.cpp
        src/Traits/Messenger.h
        src/Managers/EventsManager.h
        src/Controllers/UIController.cpp
        src/Controllers/UIController.h
        src/Managers/EventManager.cpp
        src/Managers/ComponentsManager.cpp
        src/Managers/ComponentsManager.h
        src/Managers/WindowManager.h
        src/Application/Application.h
        src/Interfaces/Component.h
        src/Adapters/UIComponentAdapter.cpp
        src/Adapters/UIComponentAdapter.h
        src/Application/Application.cpp
        src/Interfaces/Updateable.h
        src/Controllers/Controller.h
        src/Graphics/Grid.cpp
        src/Graphics/Grid.h
        src/Graphics/Shape.cpp
        src/Graphics/Shape.h
        src/Graphics/Circle.h
        src/Graphics/Convex.h
        src/Traits/Positionable.cpp
        src/Traits/Positionable.h
        src/Utilities/Tween.h
        src/Traits/PositionableTemplates.cpp
        src/Graphics/Button.cpp
        src/Graphics/Button.h
        src/Managers/FontManager.cpp
        src/Managers/FontManager.h
        src/Graphics/RectangleButton.h
        src/Graphics/CircleButton.h
        src/Graphics/ConvexButton.h
        ECS/EventManagerECS.cpp
        ECS/EventManagerECS.h
        ECS/HealthSystem.cpp
        ECS/HealthSystem.h
        ECS/ECSManager.cpp
        ECS/ECSManager.h
        ECS/EntitiesAndComponents.h
        ECS/MovementSystem.cpp
        ECS/MovementSystem.h
        ECS/RenderingSystem.cpp
        ECS/RenderingSystem.h
        ECS/SizeSystem.cpp
        ECS/SizeSystem.h
        src/Managers/UIManagerEDA.cpp
        src/Managers/UIManagerEDA.h
        src/Managers/UIManagerMVC.cpp
        src/Managers/UIManagerMVC.h
        src/Managers/Manager.h
        src/Graphics/Text.h
        src/Graphics/Sprite.h
        src/Utilities/TextManipulators.cpp
        src/Utilities/TextManipulators.h
        src/Graphics/Text.cpp
        src/Graphics/TextInput.cpp
        src/Graphics/TextInput.h
        src/Graphics/Cursor.cpp
        src/Graphics/Cursor.h
        src/Graphics/TextBox.cpp
        src/Graphics/TextBox.h
        examples/BasicExample/BasicExample.cpp
        examples/BasicExample/BasicExample.h
        src/Graphics/Typer.cpp
        src/Graphics/Typer.h
        Malena/common.hpp
        #        src/Graphics/VertexArray.cpp
#        src/Graphics/VertexArray.h
)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
