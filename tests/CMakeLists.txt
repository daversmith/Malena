enable_testing()

# Cascade regression test — exercises today's Core/Component cascade.
add_executable(EnableCascadeTest EnableCascade/main.cpp)
target_link_libraries(EnableCascadeTest PRIVATE Malena::Malena)
target_include_directories(EnableCascadeTest PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME EnableCascade COMMAND EnableCascadeTest)

# Throwaway spike for the children-unification refactor. Pure standard library,
# no Malena dependency. Delete this directory once the refactor lands.
add_executable(CoreUnifySpike CoreUnifySpike/main.cpp)
target_compile_features(CoreUnifySpike PRIVATE cxx_std_17)
add_test(NAME CoreUnifySpike COMMAND CoreUnifySpike)
