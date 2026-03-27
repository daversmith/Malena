add_executable(UIControls UIControls.cpp UIControls.h
        main.cpp)

target_link_libraries(UIControls PRIVATE Malena::Malena)

target_include_directories(UIControls PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Copy the assets folder next to the executable after every build.
# Without this, AssetsManager::get() can't find the font file at runtime.
add_custom_command(TARGET UIControls POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:UIControls>/assets"
)
