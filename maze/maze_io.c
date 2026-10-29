/**
 * @file maze_io.c
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-29
 */

#include <stdio.h>
#include <stdlib.h>

#include "maze_io.h"


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


Maze loadMazeFromJson(FILE *f, int *size) {
    *size = 0;
    fscanf(f, "{\"size\":%d,\"walls\":[", size);
    Maze maze = malloc(*size * sizeof(Cell*));
    for (int i = 0; i < *size; i++) {
        maze[i] = malloc(*size * sizeof(Cell));
    }
    for (int y = 0; y < *size; y++) {
        if (y > 0)
            fscanf(f, ",");
        fscanf(f, "[");
        for (int x = 0; x < *size; x++) {
            if (x > 0)
                fscanf(f, ",");
            int v;
            fscanf(f, "%d", &v);
            maze[y][x] = v;
        }
        fscanf(f, "]");
    }
    return maze;
}