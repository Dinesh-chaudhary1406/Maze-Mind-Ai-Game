#pragma once

#include <vector>
#include <string>

// 0 = top, 1 = right, 2 = bottom, 3 = left
struct Cell {
    int x;
    int y;
    bool visited;
    bool walls[4];
};

class Maze {
public:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    Maze(int w, int h);

    // Generate a perfect maze using Prim's algorithm.
    void generate();

    // Serialize the maze into a JSON string.
    std::string toJSON() const;
};

