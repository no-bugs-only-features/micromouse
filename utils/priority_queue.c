/**
 * @file priority_queue.c
 * @brief Implementation of a priority queue of maze cells.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.2
 * @date 2026-09-28
 */

#include <stdint.h>
#include <stdlib.h>

#include "maze.h"
#include "priority_queue.h"


PriorityQueue* createPriorityQueue(int size) {
    PriorityQueue* pq = (PriorityQueue*) malloc(sizeof(PriorityQueue));
    pq->max_size = size * size;
    pq->entries = (PQEntry*) malloc(pq->max_size * sizeof(PQEntry));
    pq->position = (uint16_t*) malloc(pq->max_size * sizeof(uint16_t));
    /* Initialize all positions to UINT16_MAX to indicate that they are not in the queue. */
    for (int i = 0; i < pq->max_size; i++) {
        pq->position[i] = UINT16_MAX;
    }
    pq->size = 0;
    return pq;
}


void destroyPriorityQueue(PriorityQueue* pq) {
    free(pq->entries);
    free(pq->position);
    free(pq);
}


void swap(PriorityQueue* pq, int a, int b) {
    PQEntry temp = pq->entries[a];
    pq->entries[a] = pq->entries[b];
    pq->entries[b] = temp;
    pq->position[pq->entries[a].cell_id] = a;
    pq->position[pq->entries[b].cell_id] = b;
}


void heapifyUp(PriorityQueue* pq, int index) {
    if (index <= 0) return;
    int parent = (index - 1) / 2;
    if (pq->entries[parent].priority > pq->entries[index].priority) {
        swap(pq, parent, index);
        heapifyUp(pq, parent);
    }
}


void heapifyDown(PriorityQueue* pq, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < pq->size && pq->entries[left].priority < pq->entries[smallest].priority) {
        smallest = left;
    }

    if (right < pq->size && pq->entries[right].priority < pq->entries[smallest].priority) {
        smallest = right;
    }

    if (smallest != index) {
        swap(pq, index, smallest);
        heapifyDown(pq, smallest);
    }
}


void decreasePriority(PriorityQueue* pq, CellID cell_id, uint8_t new_priority) {
    int index = pq->position[cell_id];
    if (index != UINT16_MAX && index < pq->size && pq->entries[index].priority > new_priority) {
        pq->entries[index].priority = new_priority;
        heapifyUp(pq, index);
    }
}


void enqueue(PriorityQueue* pq, CellID cell_id, uint8_t priority) {
    if (pq->size >= pq->max_size) return;
    pq->entries[pq->size].cell_id = cell_id;
    pq->entries[pq->size].priority = priority;
    pq->position[cell_id] = pq->size;
    pq->size++;
    heapifyUp(pq, pq->size - 1);
}


CellID dequeue(PriorityQueue* pq) {
    if (pq->size <= 0) return UINT16_MAX;
    PQEntry top = pq->entries[0];
    swap(pq, 0, --pq->size);
    pq->position[top.cell_id] = UINT16_MAX;
    if (pq->size > 0) heapifyDown(pq, 0);
    return top.cell_id;
}


CellID peek(PriorityQueue* pq) {
    if (pq->size <= 0) return UINT16_MAX;
    return pq->entries[0].cell_id;
}