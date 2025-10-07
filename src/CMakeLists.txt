file(GLOB_RECURSE MALENA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(Malena ${MALENA_SOURCES})

add_library(Malena::Malena ALIAS Malena)

# Set include directories
target_include_directories(Malena
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard
target_compile_features(Malena PUBLIC cxx_std_17)

# Link with SFML
target_link_libraries(Malena PUBLIC SFML::Graphics)

# Install the library
install(TARGETS Malena
        EXPORT Malena-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export the targets
install(
        EXPORT Malena-targets
        FILE Malena-targets.cmake
        NAMESPACE Malena::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Malena
)