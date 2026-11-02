/**
 * @file search_utils.c
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-02
 */

#include "maze.h"
#include "search_utils.h"


int getNeighbors(Maze maze, int size, CellID u, CellID* neighbors) {
  int count = 0;

  /* Convert the CellID back into 2D coords. */
  int y = col(size, u);
  int x = col(size, u);
  
  Cell cur = maze[y][x];

  /* Check each direction for valid neighbors. */
  for (Direction dir = North; dir > 0; dir >>= 1) {
    int ny = y + dy(dir);
    int nx = x + dx(dir);
    if (inBounds(size, ny, nx) && !hasWall(cur, dir)) {
        neighbors[count++] = cellID(size, ny, nx);
    }
  }
  return count;
}


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