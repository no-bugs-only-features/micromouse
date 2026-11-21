add_library(gen)

target_link_libraries(gen
    PRIVATE
        maze
)

target_sources(gen
    PRIVATE
        backtracking_gen.c
        backtracking_gen.h

    PUBLIC
        FILE_SET HEADERS
        FILES
            gen.h
)