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
#include "nav.h"
#include "search.h"

action_t mouse_step(Cell walls, State cur_state) {
    CellID goal = cellID(cur_state.size, cur_state.size / 2, cur_state.size / 2);

    /* If direction is unknown. */
    if (!*cur_state.dir) {
        /* Check for an opening to the left or right to determine direction. */
        if (~walls & (1 | 4)) {
            *cur_state.dir = ~walls & 1 ? South : East;

            /* Initialize some stuff. */
            *cur_state.pos = cellID(cur_state.size, *cur_state.pending_len * dy(*cur_state.dir), *cur_state.pending_len * dx(*cur_state.dir));

            /* Rotate all pending cells and put them into the maze. */
            for (int i = 0; i < *cur_state.pending_len; i++) {
                int y = i * dy(*cur_state.dir);
                int x = i * dx(*cur_state.dir);
                Cell o = orient(cur_state.pending[i], cur_state.size, *cur_state.dir);
                /* Set both walls in the maze. */
                for (Direction d = North; d; d >>= 1) {
                    if (hasWall(o, d)) setWallBoth(cur_state.maze, cur_state.size, y, x, d);
                }
            }

            /* Run a full flood of the maze to initialize distances. */
            fullFlood(cur_state.maze, cur_state.size, 0, goal, cur_state.dist);

        } else {
            cur_state.pending[(*cur_state.pending_len)++] = walls;
            return FORWARD;
        }
    }

    /* Add oriented wall data to maze. */
    Cell o = orient(walls, cur_state.size, *cur_state.dir);
    /* Set both walls in the maze. */
    for (Direction d = North; d; d >>= 1) {
        if (hasWall(o, d))
            setWallBoth(cur_state.maze, cur_state.size, row(cur_state.size, *cur_state.pos), col(cur_state.size, *cur_state.pos), d);
    }

    /* Update distances using flood fill. */
    modifiedFlood(cur_state.maze, cur_state.size, *cur_state.pos, goal, cur_state.dist);

    /* Get downhill path to goal. */
    int path_len = downhillPath(cur_state.maze, cur_state.size, cur_state.dist, *cur_state.pos, *cur_state.dir, cur_state.path);

    action_t action;
    
    /* Path length of 1 indicates we are at the goal, or we cannot reach it. */
    if (path_len <= 1) {
        action = DONE;
    } else {
        /* Translate the next move in the path to an action. */
        CellID next = cur_state.path[1];

        /* This logic should be improved in the actual mouse. We are limited by the fact
        that we can only send one action per step back to the simulator. This means we have to 
        run A* once in order to turn and once more in order to move. */

        /* Is the next cell in the same direction? */
        int dr = row(cur_state.size, next) - row(cur_state.size, *cur_state.pos);
        int dc = col(cur_state.size, next) - col(cur_state.size, *cur_state.pos);
        Direction r_dir = (*cur_state.dir >> 1 | *cur_state.dir << 3) & 0x0F;
        Direction l_dir = opposite(r_dir);
        if (dr == dy(*cur_state.dir) && dc == dx(*cur_state.dir)) {
            *cur_state.pos = next;
            action = FORWARD;
        /* Is the next cell in the opposite direction? */
        } else if (dr == dy(opposite(*cur_state.dir)) && dc == dx(opposite(*cur_state.dir))) {
            *cur_state.dir = opposite(*cur_state.dir);
            action = TURN_180;
        /* Is the next cell in the right direction? */
        } else if (dr == dy(r_dir) && dc == dx(r_dir)) {
            *cur_state.dir = r_dir;
            action = TURN_R;
        /* Otherwise, the next cell must be to the left. */
        } else {
            *cur_state.dir = l_dir;
            action = TURN_L;
        }
    }
    return action;
}