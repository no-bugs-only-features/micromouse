/**
 * @file priority_queue.h
 * @brief Defines data structures and functions for a priority queue of maze cells.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-09-28
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>

#include "cell.h"

/*
 * The size of the maze (16x16).
 */
#define MAX 256


/**
 * @brief An entry in the priority queue.
 */
typedef struct {
    CellID cell_id; /** ID of the cell. */
    uint8_t priority; /** Priority of the cell. */
} PQEntry;


/**
 * @brief A priority queue for maze cells.
 */
typedef struct {
    PQEntry entries[MAX]; /** Array of priority queue entries. */
    uint16_t position[MAX]; /** Maps cell IDs to their positions in the heap. */
    int size; /** Current number of elements in the priority queue. */
} PriorityQueue;


/**
 * @brief Swap the values of two priority queue entries.
 * 
 * @param pq Priority queue pointer.
 * @param a Index of the first priority queue entry.
 * @param b Index of the second priority queue entry.
 */
void swap(PriorityQueue* pq, int a, int b);


/**
 * @brief Maintains heap property on insertion.
 * 
 * Restore the heap property of the priority queue
 * on insertion by moving the newly inserted node
 * towards the root.
 * 
 * @param pq Priority queue pointer.
 * @param index Index of the node to move.
 */
void heapifyUp(PriorityQueue* pq, int index);


/**
 * @brief Maintains heap property on removal.
 * 
 * Restore the heap property of the priority queue
 * on removal by moving the node being removed
 * away from the root.
 * 
 * @param pq Priority queue pointer.
 * @param index Index of the node to move.
 */
void heapifyDown(PriorityQueue* pq, int index);


/**
 * @brief Decrease the priority of a specific cell in the priority queue.
 * 
 * @param pq Priority queue pointer.
 * @param cell_id ID of the cell whose priority should be decreased.
 * @param new_priority The new priority value for the cell.
 */
void decreasePriority(PriorityQueue* pq, CellID cell_id, uint8_t new_priority);


/**
 * @brief Insert a cell into the priority queue.
 * 
 * @param pq Priority queue pointer.
 * @param cell_id ID of the cell to insert.
 * @param priority Priority of the cell.
 */
void enqueue(PriorityQueue* pq, CellID cell_id, uint8_t priority);


/**
 * @brief Remove the highest-priority cell from the queue.
 * 
 * @param pq Priority queue pointer.
 * @return CellID ID of the cell removed from the queue.
 */
CellID dequeue(PriorityQueue* pq);


/**
 * @brief View the highest-priority cell in the queue.
 * 
 * @param pq Priority queue pointer.
 * @return CellID ID of the highest-priority cell in the queue.
 */
CellID peek(PriorityQueue* pq);

#endif /* PRIORITY_QUEUE_H */