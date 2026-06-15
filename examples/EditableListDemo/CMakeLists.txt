add_executable(EditableListDemo main.cpp)

target_link_libraries(EditableListDemo PRIVATE Malena::Malena)

# Homebrew GStreamer (linked into Malena) is arm64-only on Apple Silicon.
if(APPLE)
    set_target_properties(EditableListDemo PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(EditableListDemo PRIVATE ${CMAKE_SOURCE_DIR}/include)
