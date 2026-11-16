add_library(nav)

target_sources(nav
    PRIVATE
        nav.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            nav.h
)

target_link_libraries(nav
    PRIVATE
        maze
)