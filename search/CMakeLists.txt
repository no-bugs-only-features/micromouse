add_library(search)

target_sources(search
    PRIVATE
        search.c
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            search.h
)

target_link_libraries(search
    PRIVATE
        a_star
        djikstra
        maze
)

add_library(a_star)

target_sources(a_star
    PRIVATE
        a_star.c
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            a_star.h
)

target_link_libraries(a_star
    PRIVATE
        maze
        utils
)

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
        maze
        utils
)