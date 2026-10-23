/**
 * @file djikstra.c
 * @brief C implementation of the djikstra algorithm
 * @author Dylan Wright (no-bugs-only-features)
 * @version 0.1
 * @date 2026-09-28
 * 
 */

#include "cell.h"
#include "priority_queue.h"

/*
function Dijkstra(Graph, source):
    for each vertex v in Graph:
        dist[v] <- INFINITY
        prev[v] <- UNDEFINED

    dist[source] <- 0

    PQ <- empty min-priority queue
    PQ.insert(source, 0)

    while PQ is not empty:
        u <- PQ.extract_min()

        for each neighbor v of u:
            alt <- dist[u] + weight(u, v)

            if alt < dist[v]:
                dist[v] <- alt
                prev[v] <- u
                PQ.insert(v, alt)        // or decrease_key(v, alt) if supported

    return dist, prev



function ShortestPath(prev, target):
    path <- empty list
    u <- target

    while u is not UNDEFINED:
        path.prepend(u)
        u <- prev[u]

    return path
*/