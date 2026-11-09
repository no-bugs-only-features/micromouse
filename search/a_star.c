/**
 * @file a_star.c
 * @brief A* pathfinding algorithm implementation.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-30
 */
#include <stdint.h>
#include <stdlib.h>

#include "a_star.h"
#include "maze.h"
#include "priority_queue.h"
#include "search_utils.h"

void aStar(Maze maze, int size, CellID* prev) {
    CellID start = cellID(size, 0, 0);
    CellID goal = cellID(size, size / 2, size / 2);

    PriorityQueue *open = createPriorityQueue(size);
    enqueue(open, start, 0);

    for (int i = 0; i < size * size; i++) {
        prev[i] = UINT16_MAX;
    }

    int *gScore = malloc(size * size * sizeof(int));
    int *fScore = malloc(size * size * sizeof(int));
    for (int i = 0; i < size * size; i++) {
        gScore[i] = INT_MAX;
        fScore[i] = INT_MAX;
    }

    gScore[start] = 0;
    fScore[start] = heuristic(size, start, goal);

    bool *closed = malloc(size * size * sizeof(bool));
    for (int i = 0; i < size * size; i++) {
        closed[i] = false;
    }

    while (!isEmpty(open)) {
        CellID cur = dequeue(open);

        if (cur == goal)
            /* Done. */
            break;

        closed[cur] = true;

        CellID neighbors[4];
        int neighbors_c = getNeighbors(maze, size, cur, neighbors);
        for (int i = 0; i < neighbors_c; i++) {
            CellID n = neighbors[i];
            if (closed[n])
                continue;

            /* For now, we assume the cost of moving one cell is 1. */
            int tentative_g = gScore[cur] + 1;

            if (tentative_g < gScore[n]) {
                /* We found a better path to this neighbor. */
                prev[n] = cur;
                gScore[n] = tentative_g;
                fScore[n] = tentative_g + heuristic(size, n, goal);
                if (contains(open, n))
                    decreasePriority(open, n, fScore[n]);
                else
                    enqueue(open, n, fScore[n]);
            }

        }
    }

    destroyPriorityQueue(open);
    free(gScore);
    free(fScore);
}