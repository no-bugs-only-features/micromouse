/**
 * @file mouse_test.c
 * @brief Test djikstra algorithm.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-09-29
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "djikstra.h"
#include "maze.h"
#include "maze_io.h"

int main(int argc, char* argv[]) {
    FILE* f = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    /* Load the maze. */
    int size;
    Maze maze = loadMazeFromJson(f, &size);

    fclose(f);

    /* Run the algorithm. */
    uint8_t* dist = malloc(size * size * sizeof(uint8_t));
    CellID* prev = malloc(size * size * sizeof(CellID));
    djikstra(maze, size, dist, prev);

    /* Generate backtracking path. */
    CellID* path = malloc(size * size * sizeof(CellID));
    int path_length = reconstructPath(prev, cellID(size, size/2, size/2), path);

    /* Write the solution. */
    f = fopen(argv[1], "w");
    if (f == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    writeMazeToJson(f, maze, size, path, path_length);
    fclose(f);

    /* Clean up. */
    free(path);
    free(prev);
    free(dist);
    for (int i = 0; i < size; i++) {
        free(maze[i]);
    }
    free(maze);

    return EXIT_SUCCESS;
}