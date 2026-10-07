/**
 * @file priority_queue.c
 * @brief Implementation of a priority queue of maze cells.
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-09-28
 */

#include <stdint.h>

#include "cell.h"
#include "priority_queue.h"


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
    if (index >= 0 && index < pq->size && pq->entries[index].priority > new_priority) {
        pq->entries[index].priority = new_priority;
        heapifyUp(pq, index);
    }
}


void enqueue(PriorityQueue* pq, CellID cell_id, uint8_t priority) {
    if (pq->size >= MAX) return;
    pq->entries[pq->size].cell_id = cell_id;
    pq->entries[pq->size].priority = priority;
    pq->size++;
    heapifyUp(pq, pq->size - 1);
}


CellID dequeue(PriorityQueue* pq) {
    if (pq->size <= 0) return 0;
    PQEntry top = pq->entries[0];
    pq->entries[0] = pq->entries[--pq->size];
    heapifyDown(pq, 0);
    return top.cell_id;
}


CellID peek(PriorityQueue* pq) {
    if (pq->size <= 0) return 0;
    return pq->entries[0].cell_id;
}