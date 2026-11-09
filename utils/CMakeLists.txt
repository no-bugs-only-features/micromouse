add_library(utils)

target_sources(utils
    PRIVATE
        priority_queue.c
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            priority_queue.h
)

target_link_libraries(utils
    PRIVATE
        maze
)