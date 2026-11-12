/**
 * @file flood_fill.c
 * @brief Full and modified flood fill algorithm implementation.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-11
 */
#include <stdint.h>

#include "flood_fill.h"
#include "maze.h"
#include "priority_queue.h"


void fullFlood(Maze maze, int size, CellID start, CellID goal, uint16_t* dist) {
    for (int i = 0; i < size * size; i++) {
        dist[i] = UINT16_MAX;
    }

    PriorityQueue *pq = createPriorityQueue(size);
    dist[goal] = 0;
    enqueue(pq, goal, 0);

    while(!isEmpty(pq)) {
        CellID u = dequeue(pq);
        CellID neighbors[4];
        int neighbors_c = getNeighbors(maze, size, u, neighbors);
        for (int i = 0; i < neighbors_c; i++) {
            CellID n = neighbors[i];
            if (dist[n] == UINT16_MAX) {
                dist[n] = dist[u] + 1;
                enqueue(pq, n, 0);
            }
        }
    }
}


void modifiedFlood(Maze maze, int size, CellID start, CellID goal, uint16_t* dist) {
    CellID stack[256];
    int stack_c = 0;
    stack[stack_c++] = start;

    while(stack_c > 0) {
        CellID u = stack[stack_c--];
        if (u == goal)
            continue;

        CellID neighbors[4];
        int neighbors_c = getNeighbors(maze, size, u, neighbors);
        CellID min_neighbor = neighbors[0];
        for (int i = 1; i < neighbors_c; i++) {
            CellID n = neighbors[i];
            if (dist[min_neighbor] < dist[n])
                min_neighbor = n;
        }
        
        if (dist[u] != dist[min_neighbor] + 1) {
            dist[u] = dist[min_neighbor] + 1;
            for (int i = 0; i < neighbors_c; i++) {
                stack[stack_c++] = neighbors[i];
            }
        }
    } 
}