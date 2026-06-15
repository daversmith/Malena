add_executable(CascadeDemo main.cpp)

target_link_libraries(CascadeDemo PRIVATE Malena::Malena)

target_include_directories(CascadeDemo PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
