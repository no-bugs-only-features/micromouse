add_library(maze)

target_sources(maze
    PRIVATE
        maze.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            maze.h
)

add_library(maze_io)

target_sources(maze_io
    PRIVATE
        maze_io.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            maze_io.h
)