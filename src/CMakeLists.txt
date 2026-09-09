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
    target_link_libraries(Malena PUBLIC "-framework AppKit")
    # ScreenSource captures the desktop with CoreGraphics (no GStreamer needed).
    target_link_libraries(Malena PRIVATE "-framework CoreGraphics")
endif()

# ScreenSource's X11 backend. Optional: without it Malena still builds and
# ScreenSource::isSupported() reports false rather than failing the build.
if(UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    if(X11_FOUND)
        target_include_directories(Malena PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(Malena PRIVATE ${X11_LIBRARIES})
        message(STATUS "Malena: ScreenSource X11 capture enabled")
    else()
        message(STATUS "Malena: X11 not found — ScreenSource capture disabled")
    endif()
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

# Native file dialogs — PRIVATE, NFD headers are only in the .cpp implementation.
# NOT wrapped in $<BUILD_INTERFACE:> : that reads as a usage requirement and still
# leaves a reference to the `nfd` target in Malena's exported interface, so
# install(EXPORT) fails with "requires target nfd that is not in any export set".
# Plain PRIVATE keeps it out of the interface for a shared build; a STATIC build
# still needs nfd at link time, which the install() below handles.
target_link_libraries(Malena PRIVATE nfd)

# nlohmann/json — header-only. BUILD_INTERFACE limits the dependency to the
# build tree so it does not appear in the install export set (which would
# require nlohmann to be installed system-wide as well).
target_link_libraries(Malena PUBLIC
    "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>")

# ── AirPlay optional dependencies ─────────────────────────────────────────────
if(MALENA_BUILD_AIRPLAY)
    target_compile_definitions(Malena PUBLIC MALENA_AIRPLAY_ENABLED)

    target_link_libraries(Malena PRIVATE
        ${AIRPLAY_MDNS_LIBS}
        "$<BUILD_INTERFACE:mbedcrypto>"
        "$<BUILD_INTERFACE:monocypher>"
        "$<BUILD_INTERFACE:playfair>")

    # VideoToolbox for H264 decode on macOS (Phase 2)
    if(APPLE)
        target_link_libraries(Malena PRIVATE
            "-framework VideoToolbox"
            "-framework CoreMedia"
            "-framework CoreVideo"
            "-framework CoreFoundation")
    endif()

    if(AIRPLAY_MDNS_INCS)
        target_include_directories(Malena PRIVATE ${AIRPLAY_MDNS_INCS})
    endif()
endif()

# ── ScreenReceiver (RTSP/H.264 → texture) via GStreamer ───────────────────────
# Independent of AirPlay. LGPL — dynamically linked, fine for closed-source apps.
if(GSTREAMER_FOUND AND GST_APP_FOUND AND GST_VIDEO_FOUND)
    target_compile_definitions(Malena PUBLIC MALENA_SCREENSHARE_ENABLED)
    target_link_libraries(Malena PRIVATE
        PkgConfig::GSTREAMER
        PkgConfig::GST_APP
        PkgConfig::GST_VIDEO)
    message(STATUS "Malena: ScreenReceiver enabled (GStreamer ${GSTREAMER_VERSION})")
elseif(MALENA_ENABLE_SCREENSHARE)
    # ASKED FOR IT AND CANNOT HAVE IT — that is an error, not a downgrade.
    #
    # This used to print a STATUS line and carry on building a Malena with no
    # ScreenReceiver in it. Every downstream signal still looked right:
    # MALENA_ENABLE_SCREENSHARE stayed ON in the cache, the app still linked,
    # the bundlers still shipped the GStreamer runtime beside it. The only way
    # to discover the feature was missing was to grep the finished binary.
    #
    # It shipped that way. LockIn's macOS student app had no receiver at all,
    # and a Windows rebuild silently lost one it previously had when a
    # reconfigure ran without GSTREAMER_1_0_ROOT set. In both cases students
    # opened the share view and got a black panel, because there was nothing
    # behind it — and the relay logs showed they never connected at all.
    #
    # A build that cannot deliver what was asked of it must say so here, while
    # someone is watching, not in a classroom.
    message(FATAL_ERROR
        "Malena: MALENA_ENABLE_SCREENSHARE=ON but GStreamer was not found.\n"
        "  Found: GSTREAMER=${GSTREAMER_FOUND} GST_APP=${GST_APP_FOUND} GST_VIDEO=${GST_VIDEO_FOUND}\n"
        "  macOS  : install the official GStreamer.framework (runtime + devel)\n"
        "  Windows: set GSTREAMER_1_0_ROOT_MSVC_X86_64 to the GStreamer install\n"
        "  Linux  : apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev\n"
        "  Or build without the feature on purpose: -DMALENA_ENABLE_SCREENSHARE=OFF")
else()
    message(STATUS "Malena: ScreenReceiver disabled (MALENA_ENABLE_SCREENSHARE=OFF)")
endif()

# nfd is a PRIVATE dependency, but CMake still records private deps of an
# exported target in its interface (as $<LINK_ONLY:...>), so install(EXPORT)
# refuses to generate unless nfd is exported too — and a consumer linking a
# STATIC Malena really does need nfd's objects at link time. Export it in the
# same set rather than pretending the dependency isn't there.
install(TARGETS nfd
        EXPORT Malena-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

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
