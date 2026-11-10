/**
 * @file djikstra.h
 * @brief Djikstra's algorithm implementation for pathfinding in a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-22
 */
#ifndef DJIKSTRA_H
#define DJIKSTRA_H

#include "maze.h"


/**
 * @brief Runs djikstras on the maze. Populates the dist and prev arrays.
 *
 * @param maze The maze.
 * @param size The size of the maze.
 * @param start The starting node.
 * @param dist Pointer to an array to store the shortest distances from the start to each cell.
 *             Array should be size * size and indexed by CellID.
 * @param prev Pointer to an array to store the previous cell in the shortest path from the start 
 *             to each cell. Array should be size * size and indexed by CellID.
 */
void djikstra(Maze maze, int size, CellID start, uint8_t* dist, CellID* prev);

#endif /* DJIKSTRA_H */