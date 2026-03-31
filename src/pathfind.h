#pragma once

#include <vector>
#include <utility>

#include "maze.h"

struct Node {
    int x;
    int y;
    float g_cost;
    float h_cost;
    float f_cost;
    Node* parent;
};

class Pathfinder {
public:
    // Dijkstra's algorithm: heuristic is always 0.
    static std::vector<std::pair<int, int>> dijkstra(
        const std::vector<std::vector<Cell>>& grid,
        int width,
        int height,
        std::pair<int, int> start,
        std::pair<int, int> end,
        std::vector<std::pair<int, int>>& visitedOrder);

    // A* search algorithm with Manhattan heuristic.
    static std::vector<std::pair<int, int>> astar(
        const std::vector<std::vector<Cell>>& grid,
        int width,
        int height,
        std::pair<int, int> start,
        std::pair<int, int> end,
        std::vector<std::pair<int, int>>& visitedOrder);

    // Return neighbors reachable from (x, y) where the wall is open.
    static std::vector<std::pair<int, int>> getNeighbors(
        const std::vector<std::vector<Cell>>& grid,
        int width,
        int height,
        int x,
        int y);
};

