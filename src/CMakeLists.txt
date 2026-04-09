file(GLOB_RECURSE MALENA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Export all symbols from the shared library.
# On MSVC this uses cmake -E __create_def; on MinGW it uses --export-all-symbols.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(Malena ${MALENA_SOURCES})

# cmake -E __create_def crashes on MSVC when LTO (LTCG) is enabled because
# it cannot parse the bitcode object files. Disable LTO for MSVC shared builds.
if(MSVC AND BUILD_SHARED_LIBS)
    set_target_properties(Malena PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)
endif()

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

# Export MALENA_API symbols on Windows when building the DLL
target_compile_definitions(Malena PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:MALENA_EXPORTS>)

# Link with SFML
target_link_libraries(Malena PUBLIC SFML::Graphics SFML::Audio
        SFML::Window
        SFML::System)

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