/**
 * @file search.h
 * @brief Umbrella header for algorithms and functions related to searching through a maze.
 *
 * Available search algorithms included by this module:
 * - @see a_star.h
 * - @see djikstra.h
 *
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef SEARCH_H
#define SEARCH_H

#include "a_star.h"
#include "djikstra.h"
#include "maze.h"


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

#endif /* SEARCH_H */