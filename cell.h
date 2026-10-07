/**
 * @file cell.h
 * @brief Defines data structures and utilities used to represent a maze cell.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-09-28
 */
#ifndef CELL_H
#define CELL_H

#include <stdint.h>


/**
 * @brief The size of the maze (16x16).
 */
#define SIZE 16

/**
 * @brief Mask for cell's wall data.
 */
#define MASK 0x0F


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
 * @brief Represents the unique identifier for a cell in the maze.
 *
 * This type is used to uniquely identify each cell within the maze.
 * It is the index of the cell within the maze grid.
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
static inline CellID cellID(uint8_t row, uint8_t col) { return row * SIZE + col; }


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
static inline void setWall(Cell *cell, Direction dir) { *cell = *cell | dir; }

/**
 * @brief Mark the specified direction as explored.
 * 
 * @param cell Pointer to the cell whose exploration state should be modified.
 * @param dir The direction in which to mark as explored.
 */
static inline void setExplored(Cell *cell, Direction dir) { *cell = *cell | (dir << 4); }

#endif /* CELL_H */