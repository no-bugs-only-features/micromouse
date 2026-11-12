/**
 * @file search.h
 * @brief Umbrella header for algorithms and functions related to searching through a maze.
 *
 * Available search sub-modules included by this module:
 * - @ref a_star.h
 * - @ref djikstra.h
 * - @ref flood_fill.h
 *
 * @see a_star.h
 * @see djikstra.h
 * @see flood_fill.h
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef SEARCH_H
#define SEARCH_H

#include "a_star.h"
#include "djikstra.h"
#include "flood_fill.h"
#include "maze.h"


/** @brief Reconstructs the shortest path from the start to the target.
 *
 * The start cell is determined by the function that populated the prev array.
 * If the path length is 0, either the target is unreachable or the target is the start.
 *
 * Available search algorithms that generate a prev array:
 * - @ref aStar
 * - @ref djikstra
 *
 * @param prev The array of previous cells.
 * @param target The target CellID.
 * @param path An array to store the shortest path.
 * @return The number of cells in the shortest path.
 */
int reconstructPath(CellID* prev, CellID target, CellID* path);

#endif /* SEARCH_H */