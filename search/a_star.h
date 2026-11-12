/**
 * @file a_star.h
 * @brief A* pathfinding algorithm definitions.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-30
 */
#ifndef A_STAR_H
#define A_STAR_H

#include "maze.h"


/**
 * @brief Returns the heuristic value between 2 cells.
 *
 * Currently, this is the manhattan distance between the 2 cells. A more complex heurisitic function 
 * can be used in the future, taking in to consideration turns, straightaways, etc.
 * 
 * @param size The size of the maze.
 * @param a The first cell.
 * @param b The second cell.
 * @return int The heuristic value between the cells.
 */
static inline int heuristic(int size, CellID a, CellID b) { manhattan(size, a, b); }


/**
 * @brief Run A* on the maze. Populates the prev array.
 * 
 * @param maze The maze.
 * @param size The size of the maze.
 * @param start The starting node to path from.
 * @param target The target node to path to.
 * @param prev Pointer to an array to store the previous cell in the shortest path from
 *             the start to the target. Array should be size * size and indexed by CellID.
 */
void aStar(Maze maze, int size, CellID start, CellID target, CellID* prev);

#endif /* A_STAR_H */