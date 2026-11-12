/**
 * @file flood_fill.h
 * @brief Full and modified flood fill algorithm definitions.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-11
 */
#ifndef FLOOD_FILL_H
#define FLOOD_FILL_H

#include <stdint.h>

#include "maze.h"


void fullFlood(Maze maze, int size, CellID start, CellID goal, uint16_t* dist);


void modifiedFlood(Maze maze, int size, CellID start, CellID goal, uint16_t* dist);

#endif /* FLOOD_FILL_H */