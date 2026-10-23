add_executable(mouse_test)

target_sources(mouse_test
    PRIVATE
        mouse_test.c
)

target_link_libraries(mouse_test
    PRIVATE
        maze
        djikstra
)