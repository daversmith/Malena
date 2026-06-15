add_executable(ScreenShareDemo
        main.cpp
        ScreenShareDemo.h
        ScreenShareDemo.cpp
)

target_link_libraries(ScreenShareDemo PRIVATE Malena::Malena)

# Homebrew GStreamer is arm64-only on Apple Silicon; build this demo arm64-only
# so the link succeeds (the rest of the universal build is unaffected).
if(APPLE)
    set_target_properties(ScreenShareDemo PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(ScreenShareDemo PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
