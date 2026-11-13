/**
 * @file mouse.h
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

#include "maze.h"


typedef enum {
    FORWARD = 0,
    TURN_L,
    TURN_R,
    TURN_180,
    DONE
} action_t;

action_t mouse_step(Cell walls, Maze maze, int size, CellID *pos, Direction *dir, CellID *path, Cell *pending, uint16_t *pending_len);

#endif /* MOUSE_H */