/**
 * @file maze_io.h
 * @brief Header file for maze input/output operations.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-29
 */
#ifndef MAZE_IO_H
#define MAZE_IO_H

#include <stdio.h>

#include "maze.h"


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
 * @param size A pointer to store the size of the loaded maze.
 * @return The loaded maze.
 */
Maze loadMazeFromJson(FILE* f, int* size);


#endif /* MAZE_IO_H */