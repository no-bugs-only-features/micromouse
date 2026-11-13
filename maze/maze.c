/**
 * @file maze.c
 * @brief Implements functions for manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-10-14
 */
#include "maze.h"


void setWallBoth(Maze maze, int size, int y, int x, Direction dir) {
    if (inBounds(size, y, x)) setWall(&maze[y][x], dir);
    int nx = x + dx(dir);
    int ny = y + dy(dir);
    if (inBounds(size, ny, nx)) setWall(&maze[ny][nx], opposite(dir));
}


void clearWallBoth(Maze maze, int size, int y, int x, Direction dir) {
    maze[y][x] &= ~dir;
    int nx = x + dx(dir);
    int ny = y + dy(dir);
    maze[ny][nx] &= ~opposite(dir);
}


int getNeighbors(Maze maze, int size, CellID u, CellID* neighbors) {
    int count = 0;

    /* Convert the CellID back into 2D coords. */
    int y = row(size, u);
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


Cell orient(Cell walls, int size, Direction dir) {
    Cell result = 0;
    switch(dir) {
        case North:
            result = walls;
            break;
        case East:
            result = (walls >> 1) | (walls << 3);
            break;
        case South:
            result = (walls >> 2) | (walls << 2);
            break;
        case West:
            result = (walls >> 3) | (walls << 1);
            break;
    }
    return result & 0x0F;
}