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
        src/MouseEvents.cpp
        src/MouseEvents.h
        src/MouseEvents_nontemplate.cpp
        src/MouseEvents_nontemplate.cpp
        src/StateManager.cpp
        src/StateManager.h
        src/Graphics/Rectangle.h
        src/UIManager.cpp
        src/UIManager.h
        src/UIComponent.cpp
        src/UIComponent.h
        src/MessageManager.cpp
        src/MessageManager.h
        src/EventsManager.h
        src/AppController.cpp
        src/AppController.h
        examples/RectangleApp.cpp
        examples/RectangleApp.h
        src/EventManager.cpp
        src/ComponentsManager.cpp
        src/ComponentsManager.h
        src/WindowManager.h
        src/Application.h
        src/Component.h
        src/UIComponentAdapter.cpp
        src/UIComponentAdapter.h
        src/Application.cpp
        src/Updateable.h
        src/Controller.cpp
        src/Controller.h
        src/Graphics/Grid.cpp
        src/Graphics/Grid.h
        src/Shape.cpp
        src/Shape.h
        src/Graphics/Circle.h
        src/Graphics/Convex.h
        src/Positionable.cpp
        src/Positionable.h
        src/Tween.h
        src/PositionableTemplates.cpp
)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
