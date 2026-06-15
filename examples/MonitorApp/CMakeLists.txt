add_executable(MonitorApp
        main.cpp
        MonitorApp.h
        MonitorApp.cpp
)

target_link_libraries(MonitorApp PRIVATE Malena::Malena)

# Homebrew GStreamer is arm64-only on Apple Silicon (compile-check on Mac only;
# on the Pi this property is ignored).
if(APPLE)
    set_target_properties(MonitorApp PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(MonitorApp PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
