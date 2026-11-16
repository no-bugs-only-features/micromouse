/** @file nav.h
 * @brief Header file for navigation functions.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef NAV_H
#define NAV_H

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

#endif /* NAV_H */