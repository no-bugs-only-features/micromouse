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
 * @brief Finds the available neighbor cells of a given CellID.
 *
 * @param maze The maze.
 * @param u Cell of the maze to check for neighbors.
 * @param neighbors Array of neighbor cells.
 * @return The number of neighbor cells.
 */
int getNeighbors(Maze maze, int size, CellID u, CellID* neighbors);


/**
 * @brief Runs djikstras on a array of Cells. Populates the dist and prev arrays.
 *
 * This algorithm will always start at the top-left cell (CellID 0).
 *
 * @param maze The maze.
 * @param dist Pointer to an array to store the shortest distances from the start to each cell.
 *             Array should be size * size and indexed by CellID.
 * @param prev Pointer to an array to store the previous cell in the shortest path to each cell.
 *             Array should be size * size and indexed by CellID.
 */
void djikstra(Maze maze, int size, uint8_t* dist, CellID* prev);


/** @brief Reconstructs the shortest path from the start to the target.
 *
 * The start of the maze is always the top-left cell (CellID 0). If the path length is 0, either
 * the target is unreachable or the target is the start.
 *
 * @param prev The array of previous cells.
 * @param target The target CellID.
 * @param path An array to store the shortest path.
 * @return The number of cells in the shortest path.
 */
int reconstructPath(CellID* prev, CellID target, CellID* path);

#endif /* DJIKSTRA_H */