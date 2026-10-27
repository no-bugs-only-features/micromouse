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

add_executable(maze_gen)

target_sources(maze_gen
    PRIVATE
        maze_gen.c
)

target_link_libraries(maze_gen
    PRIVATE
        maze
        maze_gens
)