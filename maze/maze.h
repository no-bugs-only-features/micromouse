/**
 * @file maze.h
 * @brief Defines data structures and functions for representing and manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-10-14
 */
#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>

#include "cell.h"


/**
 * @brief Represents the unique identifier for a cell in the maze.
 *
 * This type is used to uniquely identify each cell within the maze.
 * It is the index of the cell within the maze grid.
 *
 * @see cellID
 */
typedef uint16_t CellID;


/**
 * @brief Computes the unique identifier for a cell based on its row and column.
 *
 * @param row The row of the cell.
 * @param col The column of the cell.
 * @return CellID The unique identifier for the cell.
 */
static inline CellID cellID(int size, int row, int col) { return row * size + col; }


/**
 * @brief Represents a maze as a 2D array of cells.
 *
 * See @ref cell.h for details on the Cell data structure.
 *
 * A maze has the following properties:
 * - It is always square, with a side length of `size`.
 * - The goal is always located at the center of the maze, at coordinates `(size/2, size/2)`.
 *   - If the maze has an even size, the goal occupies a 2x2 area in the center.
 * - The maze will always start at (0, 0).
 *
 * Accessing a specfic cell at (y, x) can be done using `maze[y][x]`. In the north direction,
 * the y coordinate decreases while in the south direction, the y coordinate increases. 
 * In the east direction, the x coordinate increases while in the west direction, the x coordinate decreases.
 */
typedef Cell** Maze;


/**
 * @brief Checks if the given coordinates are within the bounds of the maze.
 * 
 * @param size The size of the maze.
 * @param y The y-coordinate to check.
 * @param x The x-coordinate to check.
 * @return true if the coordinates are within the bounds, false otherwise.
 */
static inline bool inBounds(int size, int y, int x) { return 0 <= x && x < size && 0 <= y && y < size; }


/**
 * @brief Checks if the given coordinates are within the goal area of the maze.
 * 
 * @param size The size of the maze.
 * @param y The y-coordinate to check.
 * @param x The x-coordinate to check.
 * @return true if the coordinates are within the goal area, false otherwise.
 */
bool inGoal(int size, int y, int x);


/**
 * @brief Sets the wall in both directions for a given cell.
 * 
 * @param maze The maze containing the cell.
 * @param size The size of the maze.
 * @param y The y-coordinate of the cell.
 * @param x The x-coordinate of the cell.
 * @param dir The direction of the wall to set.
 */
void setWallBoth(Maze maze, int size, int y, int x, Direction dir);


/**
 * @brief Clears the wall in both directions for a given cell.
 * 
 * @param maze The maze containing the cell.
 * @param size The size of the maze.
 * @param y The y-coordinate of the cell.
 * @param x The x-coordinate of the cell.
 * @param dir The direction of the wall to clear.
 */
void clearWallBoth(Maze maze, int size, int y, int x, Direction dir);


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


/**
 * @brief Writes the maze data to a JSON file.
 * 
 * @param f The file pointer to write the JSON data to.
 * @param maze The maze to be written.
 * @param size The size of the maze.
 * @param sx The start x-coordinate of the maze.
 * @param sy The start y-coordinate of the maze.
 * @param gx The goal x-coordinate of the maze.
 * @param gy The goal y-coordinate of the maze.
 */
void writeMazeToJson(FILE* f, Maze maze, int size, CellID* path, int pathLength);


/**
 * @brief Loads a maze from a JSON file.
 * 
 * @param f The file pointer to read the JSON data from.
 * @param maze A pointer to store the loaded maze.
 * @param size A pointer to store the size of the loaded maze.
 * @return The loaded maze.
 */
Maze loadMazeFromJson(FILE* f, Maze* maze, int* size);

#endif /* MAZE_H */