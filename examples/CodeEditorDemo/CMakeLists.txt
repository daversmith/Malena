add_executable(CodeEditorDemo main.cpp)

target_link_libraries(CodeEditorDemo PRIVATE Malena::Malena)

# Homebrew GStreamer (linked into Malena) is arm64-only on Apple Silicon.
if(APPLE)
    set_target_properties(CodeEditorDemo PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(CodeEditorDemo PRIVATE ${CMAKE_SOURCE_DIR}/include)
