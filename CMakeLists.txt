cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(malena VERSION 0.1.0 LANGUAGES CXX)

# Include GNUInstallDirs for platform-specific install directories
include(GNUInstallDirs)

# Options
option(malena_BUILD_EXAMPLES "Build malena examples" OFF)
option(malena_USE_SYSTEM_SFML "Use system SFML instead of fetching it" OFF)

# SFML handling
if(malena_USE_SYSTEM_SFML)
    find_package(SFML 3.0 COMPONENTS Graphics REQUIRED)
else()
    include(FetchContent)
    FetchContent_Declare(SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.0
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()

# Add the library
add_subdirectory(src)

# Examples
if(malena_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Export targets
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/malena-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

# Create and install config file
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/malenaConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/malena-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/malena
)

# Install config files
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/malena-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/malena-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/malena
)

# Install headers
install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)