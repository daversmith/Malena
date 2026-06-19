enable_testing()

# Cascade regression test — exercises today's Core/Component cascade.
add_executable(EnableCascadeTest EnableCascade/main.cpp)
target_link_libraries(EnableCascadeTest PRIVATE Malena::Malena)
target_include_directories(EnableCascadeTest PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME EnableCascade COMMAND EnableCascadeTest)

# Layer-based child ordering — Phase 2.
add_executable(ChildOrderingTest ChildOrdering/main.cpp)
target_link_libraries(ChildOrderingTest PRIVATE Malena::Malena)
target_include_directories(ChildOrderingTest PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME ChildOrdering COMMAND ChildOrderingTest)

# Click-filter invariants — exclusive-owner gating, effective visibility through
# ancestors, enabled/Flag::ENABLED consistency (the "toolbar two-clicks" class).
add_executable(HitTestInvariantsTest HitTestInvariants/main.cpp)
target_link_libraries(HitTestInvariantsTest PRIVATE Malena::Malena)
target_include_directories(HitTestInvariantsTest PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME HitTestInvariants COMMAND HitTestInvariantsTest)

# Control-level enable/disable behavior — Select flag fix + container cascades
# (List, ScrollPane). Uses real controls (default font), still window-free.
add_executable(ControlCascadeTest ControlCascade/main.cpp)
target_link_libraries(ControlCascadeTest PRIVATE Malena::Malena)
target_include_directories(ControlCascadeTest PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME ControlCascade COMMAND ControlCascadeTest)

# ── Framework-wide subsystem suites ─────────────────────────────────────────
# Each locks the public invariants of one subsystem. Window-free.
foreach(suite
        Flags            # FlagManager + custom manifest Flag/State
        Positionable     # position, bounds, layout helpers, move queue
        CoreTopology     # reparent / remove / effective-visibility / topmostMatching
        EventSystem      # subscribe / publish / filter / exclusive owner
        Messaging        # typed MessageManager channels
        SceneManager     # bind / start / navigate / back / lazy construction
        ControlBehavior  # control value semantics (selection, values, text)
        ManifestResources) # manifest set/get + default-font singleton
    add_executable(${suite}Test ${suite}/main.cpp)
    target_link_libraries(${suite}Test PRIVATE Malena::Malena)
    target_include_directories(${suite}Test PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME ${suite} COMMAND ${suite}Test)
endforeach()
