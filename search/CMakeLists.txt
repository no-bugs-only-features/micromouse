add_library(djikstra)

target_sources(djikstra
    PRIVATE
        djikstra.c
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            djikstra.h
)

target_link_libraries(djikstra
    PRIVATE
        utils
        maze
)