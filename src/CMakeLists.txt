add_library(melena
        "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/MouseEvents_nontemplate.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/TextManipulators.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Traits/Stateful.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Traits/Messenger.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Traits/Positionable.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/UIManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/EventManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/ComponentsManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/FontManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/UIManagerEDA.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Managers/UIManagerMVC.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/UIComponent.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Controllers/UIController.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Adapters/UIComponentAdapter.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Grid.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Text.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/TextInput.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Cursor.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/TextBox.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Typer.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/EventManagerECS.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/HealthSystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/ECSManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/MovementSystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/RenderingSystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ECS/SizeSystem.cpp"
)

add_library(melena::melena ALIAS melena)

# Set include directories
target_include_directories(melena
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard
target_compile_features(melena PUBLIC cxx_std_17)

# Link with SFML
target_link_libraries(melena PUBLIC SFML::Graphics)

# Install the library
install(TARGETS melena
        EXPORT melena-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export the targets
install(
        EXPORT melena-targets
        FILE melena-targets.cmake
        NAMESPACE melena::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/melena
)