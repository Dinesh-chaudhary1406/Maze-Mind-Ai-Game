#pragma once
#include <algorithm>
#include <utility>
#include <vector>

#include "maze.h"

class Enemy {
public:
    int x;
    int y;

    Enemy(int x_, int y_) : x(x_), y(y_) {}

    // Uses BFS to find the shortest path to the player and
    // returns the next single step as {x,y}.
    // If no path exists or already at player position, returns current position.
    std::pair<int, int> nextMove(
        const std::vector<std::vector<Cell>>& grid,
        int width,
        int height,
        int player_x,
        int player_y,
        std::vector<std::pair<int, int>>& fullPath);
};

