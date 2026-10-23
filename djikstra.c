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

#include "cell.h"
#include "priority_queue.h"


/**
 * @brief Finds the available neighbor cells of a given CellID
 * @param maze the array of Cells for a given maze.
 * @param u Cell of the maze we are checking if it has neighbors.
 * @param neighbors array of given neighbor cells.
 * @return the count of the number of neighbors a cell has.
 */
int getNeighbors(Cell maze[], CellID u, CellID neighbors[]) {
  int count = 0;

  // convert the CellID back into 2D cords
  uint8_t row = u / SIZE;
  uint8_t col = u % SIZE;
  
  Cell current_cell = maze[u];

  // check north (row - 1)
  if (row > 0 && !hasWall(current_cell, North)) {
    neighbors[count++] = cellID(row - 1, col);
  }
  // check south (row + 1)
  if (row < SIZE - 1 && !hasWall(current_cell, South)) {
    neighbors[count++] = cellID(row + 1, col);
  }

  // check east (col + 1)
  if (col < SIZE - 1 && !hasWall(current_cell, East)) {
    neighbors[count++] = cellID(row, col + 1);
  }
  // check west (col - 1)
  if (col > 0 && !hasWall(current_cell, West)) {
    neighbors[count++] = cellID(row, col - 1);
  }

  return count;
}


/**
 * @brief runs djikstras on a array of Cells, reconstructs shortest path to standard out
 *
 * @param maze the array of Cells for a given maze.
 * @param start the starting cellid
 * @param target the target cellid we want to reach
 */
void djikstra(Cell maze[], CellID start, CellID target) {
  
  // dist[] tracks the distance it takes to get to any cell from the start
  uint8_t dist[MAX];
  // prev constructs the path as we move through the maze
  CellID prev[MAX];

  PriorityQueue pq;
  pq.size = 0;

  // set each CellID distance and previous to max values
  for (int v = 0; v < MAX; v++) {
    dist[v] = 255; // MAX here will cause problems with overflow
    prev[v] = MAX;
    pq.position[v] = MAX;
  }

  dist[start] = 0;

  enqueue(&pq, start, 0);


  // search the maze while there is reachable cells to explore
  while (pq.size > 0) {
    // pull off the cell with the lowest distance
    CellID u = dequeue(&pq);

    if (u == target) {
      break;
    }

    CellID neighbors[4];
    int num_neighbors = getNeighbors(maze, u, neighbors);

    for (int i = 0; i < num_neighbors; i++) {
      CellID v = neighbors[i];
      uint8_t alt = dist[u] + 1; // weight is 1 for moving one cell

      if (alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;

        //update queue
        if ( pq.position[v] != MAX
          && pq.position[v] < pq.size
          && pq.entries[pq.position[v]].cell_id == v
        ) {
          decreasePriority(&pq, v, alt);
        } else {
          enqueue(&pq, v, alt);
        }
      }
    }
  }


  if (dist[target] == 255) {
      printf("No path exists to the target!\n");
      return;
  }

  printf("Shortest path takes %d steps.\n", dist[target]);

  // reconstruction of djikstras

  // Create an array to hold the route
  CellID route[MAX];
  int step_count = 0;
  
  // Start at the target and walk backwards using the 'prev' array
  CellID current = target;
  while (current != MAX) {
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


int main() {
  // empty maze (all 0s = no walls)
  Cell maze[MAX] = {0}; 

  // Build a vertical wall down column 2
  // We will block rows 0, 1, and 2. 
  // To do this, we put East walls on Col 1, and West walls on Col 2.

  // Block Row 0
  setWall(&maze[cellID(0, 1)], East);
  setWall(&maze[cellID(0, 2)], West);

  // Block Row 1
  setWall(&maze[cellID(1, 1)], East);
  setWall(&maze[cellID(1, 2)], West);

  // Block Row 2
  setWall(&maze[cellID(2, 1)], East);
  setWall(&maze[cellID(2, 2)], West);

  // 3. Define our start (top left) and target (top right side of the wall)
  CellID start = cellID(0, 0); 
  CellID target = cellID(0, 4);

  printf("Running Dijkstra's Algorithm...\n\n");
  djikstra(maze, start, target);

  return 0;
}


