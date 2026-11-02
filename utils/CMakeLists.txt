add_library(utils)

target_sources(utils
    PRIVATE
        priority_queue.c
        search_utils.c
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            priority_queue.h
            search_utils.h
)

target_link_libraries(utils
    PRIVATE
        maze
)