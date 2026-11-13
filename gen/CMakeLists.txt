add_library(maze_gens)

target_link_libraries(maze_gens
    PRIVATE
        maze
)

target_sources(maze_gens
    PRIVATE
        backtracking_gen.c
        backtracking_gen.h

    PUBLIC
        FILE_SET HEADERS
        FILES
            maze_gens.h
)