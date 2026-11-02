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
static inline int heuristic(int size, CellID a, CellID b) { return abs(row(size, a) - row(size, b)) + abs(col(size, a) - col(size, b)); }


/**
 * @brief Run A* on the maze. Populates the prev array.
 * 
 * This algorithm will always start at the top-left cell (CellID 0) and end at the center of the maze (CellID size/2, size/2).
 * 
 * @param maze The maze.
 * @param size The size of the maze.
 * @param prev Pointer to an array to store the previous cell in the shortest path to each cell.
 *             Array should be size * size and indexed by CellID.
 */
void aStar(Maze maze, int size, CellID* prev);

#endif /* A_STAR_H */