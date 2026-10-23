add_library(maze)

target_sources(maze
    PRIVATE
        maze.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            maze.h
            cell.h
)