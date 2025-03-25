add_executable(BasicExample
        BasicExample.cpp
        BasicExample.h
        main.cpp
)

target_link_libraries(BasicExample PRIVATE Malena::Malena)

# You can also use this more explicit approach instead of the include_directories in the parent CMake
target_include_directories(BasicExample PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)