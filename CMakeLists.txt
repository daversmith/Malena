cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(Malena VERSION 0.2.2 LANGUAGES CXX)

# Include GNUInstallDirs for platform-specific install directories
include(GNUInstallDirs)

# Options
option(MALENA_BUILD_EXAMPLES "Build Malena examples" ON)
option(MALENA_USE_SYSTEM_SFML "Use system SFML instead of fetching it" OFF)

# FetchContent is used for SFML, networking, and JSON dependencies
include(FetchContent)

# SFML handling
if(MALENA_USE_SYSTEM_SFML)
    find_package(SFML 3.0 COMPONENTS Graphics Audio Window System REQUIRED)
else()
    FetchContent_Declare(SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.2
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()

# ── cpp-httplib — header-only HTTP/HTTPS client ───────────────────────────────
FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.18.3
        GIT_SHALLOW ON
        SYSTEM)
set(HTTPLIB_COMPILE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(httplib)

# ── IXWebSocket — WebSocket client ────────────────────────────────────────────
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)
    set(USE_ZLIB OFF CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG v11.4.5
        GIT_SHALLOW ON
        SYSTEM)
set(_saved_build_shared ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ixwebsocket)
set(BUILD_SHARED_LIBS ${_saved_build_shared} CACHE BOOL "" FORCE)

# ── nlohmann/json ─────────────────────────────────────────────────────────────
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW ON
        SYSTEM)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# ── nativefiledialog-extended — native OS file open/save dialogs ──────────────
FetchContent_Declare(
        nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG v1.2.1
        GIT_SHALLOW ON
        SYSTEM)
FetchContent_MakeAvailable(nfd)

# ── AirPlay receiver support ─────────────────────────────────────────────────
# DISABLED: the AirPlay mirror receiver depends on the GPLv3 `playfair` FairPlay
# code (DRM circumvention) which cannot ship in a distributed product. The code
# is archived at MalenaDeferred-airplay-archive/. Screen sharing now uses the
# clean RTSP/GStreamer ScreenReceiver instead. Forced OFF so the block below
# (mbedtls/monocypher/playfair/dns_sd) is skipped entirely.
set(MALENA_BUILD_AIRPLAY OFF)
if(MALENA_BUILD_AIRPLAY)
    # mbedTLS — source-only, no system install required.
    # CMake deduplicates FetchContent by name, so this is safe when the parent
    # project (e.g. TappedIn) also fetches mbedtls.
    if(NOT TARGET mbedcrypto)
        set(ENABLE_TESTING  OFF CACHE BOOL "" FORCE)
        set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
        set(MBEDTLS_FATAL_WARNINGS OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            mbedtls
            GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
            GIT_TAG        v3.6.0
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(mbedtls)

    endif()

    # Playfair — FairPlay AES key decryption (from UxPlay, public domain reverse-engineering).
    # Compiled as C++ to avoid needing C language support.
    if(NOT TARGET playfair)
        add_library(playfair STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/playfair.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/omg_hax.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/hand_garble.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/modified_md5.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/sap_hash.c)
        target_include_directories(playfair PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair)
        set_source_files_properties(
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/playfair.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/omg_hax.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/hand_garble.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/modified_md5.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AirPlay/playfair/sap_hash.c
            PROPERTIES LANGUAGE CXX)
    endif()

    # Monocypher — tiny, audited Ed25519+SHA-512 (libsodium-compatible).
    # Use FetchContent_Populate (not MakeAvailable) so monocypher's own CMakeLists.txt
    # is NOT processed — that CMakeLists enables the C language which triggers Objective-C
    # detection on macOS and breaks the build.  Instead we create the target manually and
    # compile the C sources as C++ (monocypher is clean C99, compatible with C++17).
    if(NOT TARGET monocypher)
        FetchContent_Declare(
            monocypher
            GIT_REPOSITORY https://github.com/LoupVaillant/Monocypher.git
            GIT_TAG        4.0.2
            GIT_SHALLOW    TRUE
        )
        FetchContent_GetProperties(monocypher)
        if(NOT monocypher_POPULATED)
            FetchContent_Populate(monocypher)
        endif()
        add_library(monocypher STATIC
            ${monocypher_SOURCE_DIR}/src/monocypher.c
            ${monocypher_SOURCE_DIR}/src/optional/monocypher-ed25519.c)
        target_include_directories(monocypher PUBLIC
            ${monocypher_SOURCE_DIR}/src
            ${monocypher_SOURCE_DIR}/src/optional)
        # Compile as C++ — avoids declaring C in LANGUAGES (which triggers OBJC on macOS)
        set_source_files_properties(
            ${monocypher_SOURCE_DIR}/src/monocypher.c
            ${monocypher_SOURCE_DIR}/src/optional/monocypher-ed25519.c
            PROPERTIES LANGUAGE CXX)
    endif()

    if(APPLE)
        # On macOS, dns_sd is a TBD stub inside the Xcode SDK — find_library
        # cannot locate it as a regular dylib.  Link via CoreServices instead,
        # which is the standard approach used by UxPlay and other Bonjour apps.
        find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
        set(AIRPLAY_MDNS_LIBS ${CORESERVICES_LIBRARY})
        set(AIRPLAY_MDNS_INCS "")

    elseif(UNIX)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(AVAHI_DNS REQUIRED avahi-compat-libdnssd)
        set(AIRPLAY_MDNS_LIBS  ${AVAHI_DNS_LIBRARIES})
        set(AIRPLAY_MDNS_INCS  ${AVAHI_DNS_INCLUDE_DIRS})

    elseif(WIN32)
        # Requires Apple Bonjour SDK for Windows: https://developer.apple.com/bonjour/
        find_library(DNSSD_LIBRARY dnssd REQUIRED)
        set(AIRPLAY_MDNS_LIBS ${DNSSD_LIBRARY} ws2_32)
        set(AIRPLAY_MDNS_INCS "")
    endif()

endif()

# GStreamer — used by the ScreenReceiver (RTSP/H.264 → sf::Texture) and the
# AirPlay Phase-2 decode. Discovered independently of AirPlay so screen-sharing
# works even when AirPlay is disabled.
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND OR PKG_CONFIG_FOUND)
    pkg_check_modules(GSTREAMER  IMPORTED_TARGET gstreamer-1.0)
    pkg_check_modules(GST_APP    IMPORTED_TARGET gstreamer-app-1.0)
    pkg_check_modules(GST_VIDEO  IMPORTED_TARGET gstreamer-video-1.0)
endif()

# Add the library
add_subdirectory(src)

# Examples
if(MALENA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Export targets
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/Malena-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

# Create and install config file
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MalenaConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Malena-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Malena
)

# Install config files
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/Malena-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/Malena-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Malena
)

# Install headers
install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)