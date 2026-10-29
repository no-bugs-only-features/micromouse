/**
 * @file maze.c
 * @brief Implements functions for manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-10-14
 */
#include "maze.h"


void setWallBoth(Maze maze, int size, int y, int x, Direction dir) {
    setWall(&maze[y][x], dir);
    int nx = x + dx(dir);
    int ny = y + dy(dir);
    setWall(&maze[ny][nx], opposite(dir));
}


void clearWallBoth(Maze maze, int size, int y, int x, Direction dir) {
    maze[y][x] &= ~dir;
    int nx = x + dx(dir);
    int ny = y + dy(dir);
    maze[ny][nx] &= ~opposite(dir);
}