/**
 * @file backtracking_gen.h
 * @brief Header file for generating mazes using the backtracking algorithm and optionally braiding them.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-12
 */
#ifndef BACKTRACKING_GEN_H
#define BACKTRACKING_GEN_H

#include "maze.h"


/**
 * @brief Generates a maze using the backtracking algorithm and optionally braids it.
 *
 * @param size The size of the maze.
 * @param seed The seed for the random number generator.
 * @param braid_pct The percentage of dead ends to remove.
 * @return Cell** The generated maze.
 */
Maze generateBacktrackingMaze(int size, int seed, int braid_pct);


/**
 * @brief Checks if a cell has not been visited in the maze generation process.
 *
 * A cell is considered unvisited if it still has all its walls intact.
 * 
 * @param cell The cell to check.
 * @return true If the cell has not been visited.
 * @return false If the cell has been visited.
 */
static inline bool isUnvisited(Cell cell) { return wallMask(cell) == 0x0F; }


/**
 * @brief Counts the number of walls present in a cell.
 * 
 * @param cell The cell whose walls are to be counted.
 * @return int The number of walls present in the cell.
 */
static inline int countWalls(Cell cell) { return __builtin_popcount(wallMask(cell)); }


/**
 * @brief Creates the goal area in the maze.
 * 
 * @param maze The maze in which to create the goal.
 * @param size The size of the maze.
 */
void createGoal(Maze maze, int size);


/**
 * @brief Opens a path to the goal area in the maze.
 * 
 * @param maze The maze in which to open the goal.
 * @param size The size of the maze.
 */
void openGoal(Maze maze, int size);

#endif /* BACKTRACKING_GEN_H */