add_executable(SignalBoard
        src/SignalBoard.cpp
        src/SignalBoard.h
        src/SignalBoardManifest.h
        src/SignalTower.h
        src/EventLog.h
        src/ScoreDisplay.h
        src/Events.h
        src/Messages.h
        src/main.cpp
)

target_link_libraries(SignalBoard PRIVATE Malena::Malena)

target_include_directories(SignalBoard PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Copy the assets folder next to the executable after every build.
# Without this, AssetsManager::get() can't find the font file at runtime.
add_custom_command(TARGET SignalBoard POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:SignalBoard>/assets"
)
