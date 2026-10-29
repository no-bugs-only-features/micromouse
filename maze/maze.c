/**
 * @file maze.c
 * @brief Implements functions for creating and manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-10-14
 */
#include <stdio.h>
#include <stdlib.h>
    
#include "maze.h"

void writeMazeToJson(FILE *f, Maze maze, int size, CellID* path, int pathLength) {
    fprintf(f, "{\"size\":%d,\"walls\":[", size);
    for (int y = 0; y < size; y++) {
        fprintf(f, "%s[", y ? "," : "");
        for (int x = 0; x < size; x++) {
            fprintf(f, "%s%d", x ? "," : "", maze[y][x]);
        }
        fprintf(f, "]");
    }
    fprintf(f, "],\"start\":{\"x\":%d,\"y\":%d},\"goal\":{\"x\":%d,\"y\":%d}",
            0, 0, size / 2, size / 2);
    if (path && pathLength > 0) {
        fprintf(f, ",\"path\":[");
        for (int i = 0; i < pathLength; i++) {
            fprintf(f, "%s{\"x\":%d,\"y\":%d}", i ? "," : "", 0, 0);
        }
        fprintf(f, "]");
    }
    fprintf(f, "}");
}


void createGoal(Maze maze, int size) {
    /* We only need to clear the goal if the maze is an even size. */
    if (size % 2 == 0) {
        int center2 = size / 2;
        int center1 = center2 - 1;

        Direction dirs[4] = { North, East, South, West };

        for (int y = center1; y <= center2; y++) {
            for (int x = center1; x <= center2; x++) {
                for (int i = 0; i < 4; i++) {
                    Direction dir = dirs[i];
                    int nx = x + dx(dir);
                    int ny = y + dy(dir);

                    if (inGoal(size, ny, nx))
                        clearWallBoth(maze, size, y, x, dir); /* Inner wall. */

                    else
                        setWallBoth(maze, size, y, x, dir); /* Outer wall. */
                }
            }
        }
    }
}


void openGoal(Maze maze, int size) {
    if (size % 2 == 0) {
        int center2 = size / 2;
        int center1 = center2 - 1;

        /* Randomly choose a corner. */
        int x = rand() % 2 ? center1 : center2;
        int y = rand() % 2 ? center1 : center2;

        /* Randomly clear an axis. */
        Direction vertical = (y == center1) ? North : South;
        Direction horizontal = (x == center1) ? West : East;
        clearWallBoth(maze, size, y, x, rand() % 2 ? vertical : horizontal);
    } else {
        /* Clear a random direction for odd-sized mazes. */
        int center = size / 2;
        clearWallBoth(maze, size, center, center, 1 << (rand() % 4));
    }
}


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

bool inGoal(int size, int y, int x) {
    if (size % 2 == 1) {
        int center = size / 2;
        return x == center && y == center;
    } else {
        int center2 = size / 2;
        int center1 = center2 - 1;
        return (x == center1 || x == center2) && (y == center1 || y == center2);
    }
}