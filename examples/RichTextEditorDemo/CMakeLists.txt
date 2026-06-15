add_executable(RichTextEditorDemo main.cpp)

target_link_libraries(RichTextEditorDemo PRIVATE Malena::Malena)

# Homebrew GStreamer (linked into Malena) is arm64-only on Apple Silicon.
if(APPLE)
    set_target_properties(RichTextEditorDemo PROPERTIES OSX_ARCHITECTURES "arm64")
endif()

target_include_directories(RichTextEditorDemo PRIVATE ${CMAKE_SOURCE_DIR}/include)
