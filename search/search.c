/**
 * @file search.c
 * @brief Implementation of functions related to searching a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#include "maze.h"
#include "search.h"


int reconstructPath(CellID* prev, CellID target, CellID* path) {
    int path_length = 0;
    for (CellID current = target; current != UINT16_MAX; current = prev[current]) {
        path[path_length++] = current;
    }

    /* Reverse the path to get it in the correct order. */
    for (int i = 0; i < path_length / 2; i++) {
        CellID temp = path[i];
        path[i] = path[path_length - 1 - i];
        path[path_length - 1 - i] = temp;
    }

    return path_length;
}