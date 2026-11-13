/**
 * @file mazegen.c
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#include <stdlib.h>

#include "maze.h"
#include "maze_gens.h"
#include "mazegen.h"


Maze generate(int size, int seed) {
    srand(seed);
    int braid_pct = rand() % 101;
    return generateBacktrackingMaze(size, seed, braid_pct);
}


void maze_free(Maze m, int size) {
    for (int i = 0; i < size; i++) {
        free(m[i]);
    }
    free(m);
}