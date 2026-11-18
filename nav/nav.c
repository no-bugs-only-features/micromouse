/**
 * @file nav.c
 * @brief Implementation of functions related to navigating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#include <stdint.h>

#include "maze.h"
#include "nav.h"


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


int downhillPath(Maze maze, int size, uint16_t *dist, CellID start, Direction start_dir, CellID *path) {
    int path_length = 0;
    CellID current = start;
    Direction cur_dir = start_dir;
    uint16_t distance = dist[current];

    while (distance > 0) {
        path[path_length++] = current;

        /* Prioritize neighbors in the same direction. */
        CellID next = getNeighbor(maze, size, current, cur_dir);
        uint16_t min_distance = next != UINT16_MAX ? dist[next] : UINT16_MAX;

        CellID neighbors[4];
        int neighbor_count = getNeighbors(maze, size, current, neighbors);

        for (int i = 0; i < neighbor_count; i++) {
            CellID neighbor = neighbors[i];
            if (dist[neighbor] < min_distance) {
                min_distance = dist[neighbor];
                next = neighbor;
            }
        }

        if (min_distance == UINT16_MAX) {
            break; // No valid next cell found, exit the loop.
        }

        if (row(size, next) == row(size, current)) {
            cur_dir = (col(size, next) > col(size, current)) ? East : West;
        } else {
            cur_dir = (row(size, next) > row(size, current)) ? South : North;
        }
        current = next;
        distance = dist[current];
    }

    return path_length;
}