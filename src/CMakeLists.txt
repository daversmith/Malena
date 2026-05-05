file(GLOB_RECURSE MALENA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# MinGW: export all remaining symbols (free functions etc.) via --export-all-symbols.
# MSVC: MALENA_API handles all exports explicitly — cmake -E __create_def not needed.
if(NOT MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

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

# On macOS, SFML Audio transitively includes Foundation framework headers (via
# CoreAudio/AudioToolbox) which contain Objective-C syntax. Compile as
# Objective-C++ so the @ directives are understood by the compiler.
if(APPLE)
    target_compile_options(Malena PUBLIC -x objective-c++)
endif()

# MALENA_EXPORTS: consumed by malena_export.h to emit dllexport when building the DLL.
target_compile_definitions(Malena PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:MALENA_EXPORTS>)

# MALENA_SHARED: propagated to consumers so malena_export.h emits dllimport.
# This is what allows MinGW to correctly resolve vtables across the DLL boundary.
target_compile_definitions(Malena PUBLIC $<$<BOOL:${BUILD_SHARED_LIBS}>:MALENA_SHARED>)

# Link with SFML
target_link_libraries(Malena PUBLIC SFML::Graphics SFML::Audio
        SFML::Window
        SFML::System)

# Networking libraries — PRIVATE because the heavy implementation details
# (httplib, ixwebsocket) are included only in .cpp files, not public headers.
target_link_libraries(Malena PRIVATE httplib::httplib ixwebsocket)

# nlohmann/json — header-only. BUILD_INTERFACE limits the dependency to the
# build tree so it does not appear in the install export set (which would
# require nlohmann to be installed system-wide as well).
target_link_libraries(Malena PUBLIC
    "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>")

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