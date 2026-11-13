/**
 * @file djikstra.c
 * @brief C implementation of the djikstra algorithm
 * @author Dylan Wright (no-bugs-only-features)
 * @author Owen Schumacher (owenschu7)
 * @version 0.2
 * @date 2026-10-22
 * 
 */
#include <stdio.h>

#include "maze.h"
#include "priority_queue.h"


void djikstra(Maze maze, int size, CellID start, uint8_t* dist, CellID* prev) {
    PriorityQueue *pq = createPriorityQueue(size);

    // set each CellID distance and previous to max values
    for (int v = 0; v < size * size; v++) {
        dist[v] = UINT8_MAX;
        prev[v] = UINT16_MAX;
    }

    dist[start] = 0;

    enqueue(pq, start, 0);


    // search the maze while there is reachable cells to explore
    while (pq->size > 0) {
        // pull off the cell with the lowest distance
        CellID u = dequeue(pq);

        CellID neighbors[4];
        int num_neighbors = getNeighbors(maze, size, u, neighbors);

        for (int i = 0; i < num_neighbors; i++) {
            CellID v = neighbors[i];
            uint8_t alt = dist[u] + 1; // weight is 1 for moving one cell

            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;

                //update queue
                if ( pq->position[v] != UINT16_MAX
                    && pq->position[v] < pq->size
                    && pq->entries[pq->position[v]].cell_id == v
                ) {
                    decreasePriority(pq, v, alt);
                } else {
                    enqueue(pq, v, alt);
                }
            }
        }
    }

  destroyPriorityQueue(pq);

}
