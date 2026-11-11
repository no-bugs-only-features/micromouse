/**
 * @file mouse.c
 * @brief 
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-11-09
 */
#include <stdint.h>
#include <stdlib.h>

#include "maze.h"
#include "mouse.h"
#include "search.h"

action_t mouse_step(Cell walls, Maze maze, int size, CellID *pos, Direction *dir, CellID *path, Cell *pending, uint16_t *pending_len) {
    /* If direction is unknown. */
    if (!(*dir)) {
        /* Check for an opening to the left or right to determine direction. */
        if (~walls & (1 | 4)) {
            *dir = ~walls & 1 ? South : East;

            /* Initialize some stuff. */
            *pos = cellID(size, *pending_len * dy(*dir), *pending_len * dx(*dir));

            /* Rotate all pending cells and put them into the maze. */
            for (int i = 0; i < *pending_len; i++) {
                int y = i * dy(*dir);
                int x = i * dx(*dir);
                Cell o = orient(pending[i], size, *dir);
                /* Set both walls in the maze. */
                for (Direction d = North; d; d >>= 1) {
                    if (hasWall(o, d)) setWallBoth(maze, size, y, x, d);
                }
            }

        } else {
            pending[(*pending_len)++] = walls;
            return FORWARD;
        }
    }

    /* Add oriented wall data to maze. */
    Cell o = orient(walls, size, *dir);
    /* Set both walls in the maze. */
    for (Direction d = North; d; d >>= 1) {
        if (hasWall(o, d))
            setWallBoth(maze, size, row(size, *pos), col(size, *pos), d);
    }

    /* Use A* to find the path to the goal. */
    CellID* prev = malloc(size * size * sizeof(CellID));
    CellID goal = cellID(size, size / 2, size / 2);
    aStar(maze, size, *pos, goal, prev);
    int path_len = reconstructPath(prev, goal, path);

    action_t action;
    
    /* Path length of 0 indicates we are at the goal, or we cannot reach it. */
    if (path_len <= 1) {
        action = DONE;
    } else {
        /* Translate the next move in the path to an action. */
        CellID next = path[1];

        /* This logic should be improved in the actual mouse. We are limited by the fact
        that we can only send one action per step back to the simulator. This means we have to 
        run A* once in order to turn and once more in order to move. */

        /* Is the next cell in the same direction? */
        int dr = row(size, next) - row(size, *pos);
        int dc = col(size, next) - col(size, *pos);
        Direction r_dir = (*dir >> 1 | *dir << 3) & 0x0F;
        Direction l_dir = opposite(r_dir);
        if (dr == dy(*dir) && dc == dx(*dir)) {
            *pos = next;
            return FORWARD;
        /* Is the next cell in the opposite direction? */
        } else if (dr == dy(opposite(*dir)) && dc == dx(opposite(*dir))) {
            *dir = opposite(*dir);
            return TURN_180;
        /* Is the next cell in the right direction? */
        } else if (dr == dy(r_dir) && dc == dx(r_dir)) {
            *dir = r_dir;
            return TURN_R;
        /* Otherwise, the next cell must be to the left. */
        } else {
            *dir = l_dir;
            return TURN_L;
        }
    }

    free(prev);
    return action;
}