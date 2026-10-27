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


int getNeighbors(Maze maze, int size, CellID u, CellID* neighbors) {
  int count = 0;

  // convert the CellID back into 2D cords
  uint8_t y = u / size;
  uint8_t x = u % size;
  
  Cell current_cell = maze[y][x];

  // check north (y - 1)
  if (y > 0 && !hasWall(current_cell, North)) {
    neighbors[count++] = cellID(y - 1, x);
  }
  // check south (y + 1)
  if (y < size - 1 && !hasWall(current_cell, South)) {
    neighbors[count++] = cellID(y + 1, x);
  }

  // check east (x + 1)
  if (x < size - 1 && !hasWall(current_cell, East)) {
    neighbors[count++] = cellID(y, x + 1);
  }
  // check west (x - 1)
  if (x > 0 && !hasWall(current_cell, West)) {
    neighbors[count++] = cellID(y, x - 1);
  }

  return count;
}


void djikstra(Maze maze, int size, CellID start, CellID target, uint8_t* dist, CellID* prev) {
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

    if (u == target) {
      break;
    }

    CellID neighbors[4];
    int num_neighbors = getNeighbors(maze, size, u, neighbors);

    for (int i = 0; i < num_neighbors; i++) {
      CellID v = neighbors[i];
      uint8_t alt = dist[u] + 1; // weight is 1 for moving one cell

      if (alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;

        //update queue
        if ( pq->position[v] != UINT16_MAX;
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

  free(pq);

  if (dist[target] == UINT8_MAX) {
      printf("No path exists to the target!\n");
      return;
  }

  printf("Shortest path takes %d steps.\n", dist[target]);

  // reconstruction of djikstras

  // Create an array to hold the route
  CellID route[size * size];
  int step_count = 0;
  
  // Start at the target and walk backwards using the 'prev' array
  CellID current = target;
  while (current != UINT16_MAX) {
      route[step_count] = current;
      step_count++;
      current = prev[current]; // Look at the breadcrumb
  }

  // The route array now holds the path, but in reverse (Target -> Start).
  // Print it looping backwards so it reads forwards (Start -> Target).
  printf("Path to take: ");
  for (int i = step_count - 1; i >= 0; i--) {
      printf("%d", route[i]);
      if (i > 0) {
          printf(" -> ");
      }
  }
  printf("\n");
}


