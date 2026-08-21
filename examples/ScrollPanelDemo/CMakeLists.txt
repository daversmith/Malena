add_executable(ScrollPanelDemo main.cpp)

target_link_libraries(ScrollPanelDemo PRIVATE Malena::Malena)

# Homebrew GStreamer (linked into Malena) is arm64-only on Apple Silicon.
if(APPLE)
    set_target_properties(ScrollPanelDemo PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(ScrollPanelDemo PRIVATE ${CMAKE_SOURCE_DIR}/include)
