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
        src/Rectangle.cpp
        src/Rectangle.h
        src/Application.cpp
        src/Application.h
        src/Application.h
        src/Component.cpp
        src/Component.h
        src/Window.h
        src/MessageManager.cpp
        src/MessageManager.h
        src/EventsManager.h
        src/AppLogic.cpp
        src/AppLogic.h
        src/RectangleApp.cpp
        src/RectangleApp.h
        src/EventManager.cpp
        src/ComponentsManager.cpp
        src/ComponentsManager.h
)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
