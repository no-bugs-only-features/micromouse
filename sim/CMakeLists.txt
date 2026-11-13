add_library(mouse SHARED)

target_sources(mouse
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/mouse.c"
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/mouse.h"
)

target_link_libraries(mouse
    PRIVATE
        maze
        search
)

add_library(mazegen SHARED)

target_sources(mazegen
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/mazegen.c"
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/mazegen.h"
)

target_link_libraries(mazegen
    PRIVATE
        gen
        maze
)