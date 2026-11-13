/**
 * @file mazegen.h
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef MAZEGEN_H
#define MAZEGEN_H

#include "maze.h"


Maze generate(int size, int seed);


void maze_free(Maze m, int size);

#endif /* MAZEGEN_H */