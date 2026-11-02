/**
 * @file search_utils.h
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-02
 */

#ifndef SEARCH_UTILS_H
#define SEARCH_UTILS_H

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

#endif /* SEARCH_UTILS_H */