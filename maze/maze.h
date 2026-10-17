/**
 * @file maze.h
 * @brief Defines data structures and functions for representing and manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-14
 */
#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>

#include "cell.h"


/**
 * @brief Represents a maze as a 2D array of cells.
 * 
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
void writeMazeToJson(FILE* f, Maze maze, int size, int sx, int sy, int gx, int gy);


/**
 * @brief Loads a maze from a JSON file.
 * 
 * @param f The file pointer to read the JSON data from.
 * @param size A pointer to store the size of the loaded maze.
 * @param sx A pointer to store the start x-coordinate of the loaded maze.
 * @param sy A pointer to store the start y-coordinate of the loaded maze.
 * @param gx A pointer to store the goal x-coordinate of the loaded maze.
 * @param gy A pointer to store the goal y-coordinate of the loaded maze.
 * @return The loaded maze.
 */
Maze loadMazeFromJson(FILE* f, int* size, int* sx, int* sy, int* gx, int* gy);

#endif /* MAZE_H */