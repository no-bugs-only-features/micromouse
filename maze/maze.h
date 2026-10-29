/**
 * @file maze.h
 * @brief Defines data structures and functions for representing and manipulating a maze.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.3
 * @date 2026-10-14
 */
#ifndef MAZE_H
#define MAZE_H

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Mask for cell's wall data.
 */
constexpr uint8_t MASK = 0x0F;


/**
 * @brief Enumerated representation of the four cardinal directions.
 */
typedef enum {
    North = 8,
    East = 4,
    South = 2,
    West = 1
} Direction;


/**
 * @brief Returns the opposite direction of the given direction.
 *
 * @param dir The direction whose opposite is to be found.
 * @return Direction The opposite direction.
 */
static inline Direction opposite(Direction dir) { return ((dir << 2) | (dir >> 2)) & 0x0F; }


/**
 * @brief Returns the change in the x-coordinate for a given direction.
 * 
 * @param dir The direction to get the x-coordinate change for.
 * @return int The change in the x-coordinate.
 */
static inline int dx(Direction dir) { return dir == East ? 1 : dir == West ? -1 : 0; }


/**
 * @brief Returns the change in the y-coordinate for a given direction.
 * 
 * @param dir The direction to get the y-coordinate change for.
 * @return int The change in the y-coordinate.
 */
static inline int dy(Direction dir) { return dir == North ? -1 : dir == South ? 1 : 0; }     


/**
 * @brief Represents a single cell in the maze.
 *
 * Every cell is an 8-bit value containing both wall and exploration
 * information.
 *
 * The four least significant bits represent whether a wall exists in each
 * direction (1=true, 0=false), while the four most significant bits represent 
 * whether that direction has been explored (1=true, 0=false).
 * 
 * The bit layout (from most significant bit to least significant bit):
 * @code
 *  +-------+-------+-------+-------+-------+-------+-------+-------+
 *  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
 *  +-------+-------+-------+-------+-------+-------+-------+-------+
 *  | North | East  | South | West  | North | East  | South | West  |
 *  +-------+-------+-------+-------+-------+-------+-------+-------+
 *  |           Explored            |             Walls             |
 *  +-------------------------------+-------------------------------+
 * @endcode
 */
typedef uint8_t Cell;


/**
 * @brief Returns the wall-state bits of a cell.
 *
 * Extracts the four least significant bits of the cell, which indicate
 * whether a wall is present in each direction.
 *
 * @param cell The cell whose wall state is queried.
 * @return A 4-bit mask containing the wall state.
 */
static inline uint8_t wallMask(Cell cell) { return cell & MASK; }

/**
 * @brief Returns the exploration-state bits of a cell.
 *
 * Extracts the four most significant bits of the cell and shifts them
 * into the lower four bits.
 *
 * @param cell The cell whose exploration state is queried.
 * @return A 4-bit mask containing the exploration state.
 */
static inline uint8_t exploredMask(Cell cell) { return (cell >> 4) & MASK; }


/**
 * @brief Checks whether a cell has a wall in a given direction.
 *
 * @param cell The cell to check.
 * @param dir The direction to check.
 * @return true A wall exists in the specified direction.
 * @return false No wall in the specified direction.
 */
static inline bool hasWall(Cell cell, Direction dir) { return wallMask(cell) & dir; }

/**
 * @brief Checks whether a cell's neighbor has been explored.
 * 
 * @param cell The cell whose neighbor should be checked.
 * @param dir The direction in which the neighbor is from the cell.
 * @return true The neighbor in the specified direction has already been explored.
 * @return false The neighbor in the specified direction has not been explored.
 */
static inline bool isExplored(Cell cell, Direction dir) { return exploredMask(cell) & dir; }


/**
 * @brief Sets a wall in the specified direction.
 *
 * @param cell Pointer to the cell whose wall state should be modified.
 * @param dir The direction in which to set the wall.
 */
static inline void setWall(Cell *cell, Direction dir) { *cell |= dir; }

/**
 * @brief Mark the specified direction as explored.
 * 
 * @param cell Pointer to the cell whose exploration state should be modified.
 * @param dir The direction in which to mark as explored.
 */
static inline void setExplored(Cell *cell, Direction dir) { *cell |= (dir << 4); }


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
 * See @ref Cell for details on the Cell data structure.
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

#endif /* MAZE_H */