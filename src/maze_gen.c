/**
 * @file maze_gen.c
 * @author Dylan Wright (no-bugs-only-features)
 * @brief 
 * @version 0.1
 * @date 2026-09-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "maze.h"
#include "maze_gens.h"


int main(int argc, char* argv[]) {

    int opt;

    int size = 16;
    int braid_pct = 0;
    int seed = -1;
    FILE* f = NULL;

    while ((opt = getopt(argc, argv, "s:b:S:")) != -1) {
        switch (opt) {
            case 's':
                size = atoi(optarg);
                if (size <= 0) {
                    fprintf(stderr, "Size must be a positive integer.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'b':
                braid_pct = atoi(optarg);
                if (braid_pct < 0 || braid_pct > 100) {
                    fprintf(stderr, "Braid percentage must be an integer between 0 and 100.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'S':
                seed = atoi(optarg);
                break;
            case ':':
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                return EXIT_FAILURE;
            default:
                fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (; optind < argc; optind++) {
        f = fopen(argv[optind], "w");
        if (f == NULL) {
            perror("fopen");
            return EXIT_FAILURE;
        }
    }

    Maze maze = generateBacktrackingMaze(size, seed, braid_pct);

    writeMazeToJson(f, maze, size, NULL, 0);

    fclose(f);

    for (int i = 0; i < size; i++) {
        free(maze[i]);
    }
    free(maze);

    return EXIT_SUCCESS;
}