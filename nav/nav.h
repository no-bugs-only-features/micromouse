/** @file nav.h
 * @brief Header file for navigation functions.
 * @see a_star.h
 * @see djikstra.h
 * @see flood_fill.h
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef NAV_H
#define NAV_H

#include <stdint.h>

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
int reconstructPath(CellID *prev, CellID target, CellID *path);


/** @brief Generates a path from the start cell a cell with a distance of 0.
 *
 * The path has bias towards cells in the current direction of travel.
 *
 * Available algorithms that generate a dist array:
 * - @ref fullFlood
 * - @ref modifiedFlood
 *
 * @param Maze The maze to navigate.
 * @param size The size of the maze.
 * @param dist The array of distances from the target.
 * @param start The starting CellID.
 * @param start_dir The current direction of travel.
 * @param path An array to store the path.
 * @return The number of cells in the path.
 */
int downhillPath(Maze maze, int size, uint16_t *dist, CellID start, Direction start_dir, CellID *path);

#endif /* NAV_H */