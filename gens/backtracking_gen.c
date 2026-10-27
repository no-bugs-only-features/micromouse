/**
 * @file backtracking_gen.c
 * @brief Generates a maze using the backtracking algorithm and optionally braids it.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-10-08
 */
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "maze.h"
#include "backtracking_gen.h"


/**
 * @brief Carves a maze using the backtracking algorithm starting from the specified cell.
 *
 * @param maze The maze to carve.
 * @param size The size of the maze.
 * @param y The y-coordinate of the starting cell.
 * @param x The x-coordinate of the starting cell.
 */
static void carve(Maze maze, int size, int y, int x);


/**
 * @brief Braids the maze by removing dead ends based on the specified percentage.
 * 
 * @param maze The maze to braid.
 * @param size The size of the maze.
 * @param braid_pct The percentage of dead ends to remove.
 */
static void braid(Maze maze, int size, int braid_pct);


Maze generateBacktrackingMaze(int size, int seed, int braid_pct) {
    srand(seed == -1 ? time(NULL) : seed);

    Maze maze = malloc(size * sizeof(Cell*));
    for (int i = 0; i < size; i++) {
        maze[i] = malloc(size * sizeof(Cell));
        for (int j = 0; j < size; j++) {
            maze[i][j] = MASK; /* Initialize all walls intact. */
        }
    }

    /* Create the goal box. */
    createGoal(maze, size);

    carve(maze, size, 0, 0);

    /* Create some cool loopdy loops. */
    if (braid_pct > 0) {
        braid(maze, size, braid_pct);
    }

    /* Open the goal box. */
    openGoal(maze, size);

    return maze;
}


static void carve(Maze maze, int size, int y, int x) {
    Direction order[4] = {North, East, South, West};

    /* Shuffle the order array using the Fisher-Yates algorithm with the seeded RNG */
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        Direction temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    /* Try each direction in the shuffled order */
    for (int i = 0; i < 4; i++) {
        Direction dir = order[i];
        int nx = x + dx(dir);
        int ny = y + dy(dir);

        /* If the neighboring cell is within bounds, unvisited, and not the goal, become Mr. Gorbachev. */
        if (0 <= nx && nx < size && 0 <= ny && ny < size && isUnvisited(maze[ny][nx]) && !inGoal(size, ny, nx)) {
            maze[y][x] &= ~dir;
            maze[ny][nx] &= ~opposite(dir);
            carve(maze, size, ny, nx);
        }
    }
}


static void braid(Maze maze, int size, int braid_pct) {
    /* A dead end is a cell with exactly 3 walls. Knocking one of its walls into
       a neighbor adds a loop, because a perfect maze already connects every
       cell, so the neighbor is always reachable another way. */
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {

            /* Check if the current cell is inside the goal. */
            if (inGoal(size, y, x))
                continue;

            /* Check if the current cell is a dead end (has exactly 3 walls). */
            if (countWalls(maze[y][x]) != 3)
                continue; /* Not a dead end. */

            if (rand() % 100 >= braid_pct) {
                continue; /* This dead end survives the hunger games. */
            }

            Direction options[4];
            int options_count = 0;

            for (int d = 1; d <= 8; d <<= 1) {
                int nx = x + dx(d);
                int ny = y + dy(d);
                if (hasWall(maze[y][x], d) && inBounds(size, ny, nx) && !inGoal(size, ny, nx)) {
                    options[options_count++] = d;
                }
            }

            /* Optionally, prefer smashing neighbors' walls when they are dead ends. */
            // Direction* joined = malloc(4 * sizeof(Direction));
            // int joined_count = 0;
            // for (int i = 0; i < options_count; i++) {
            //     int nx = x + dx(options[i]);
            //     int ny = y + dy(options[i]);
            //     if (countWalls(maze[ny][nx]) == 3)
            //         joined[joined_count++] = options[i];
            // }

            // if (joined_count > 0) {
            //     free(options);
            //     options = joined;
            //     options_count = joined_count;
            // } else {
            //     free(joined);
            // }

            if (options_count == 0)
                continue;

            Direction dir = options[rand() % options_count];
            int nx = x + dx(dir);
            int ny = y + dy(dir);

            maze[y][x] &= ~dir;
            maze[ny][nx] &= ~opposite(dir);

        }
    }
}