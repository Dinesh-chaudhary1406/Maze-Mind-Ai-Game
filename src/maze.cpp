#include "maze.h"

#include <queue>
#include <random>
#include <sstream>

namespace {

struct FrontierCell {
    int x;
    int y;
};

// Direction vectors: 0 = top, 1 = right, 2 = bottom, 3 = left
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};
const int OPPOSITE[4] = {2, 3, 0, 1};

bool inBounds(int x, int y, int w, int h) {
    return x >= 0 && x < w && y >= 0 && y < h;
}

} // namespace

Maze::Maze(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Cell& c = grid[y][x];
            c.x = x;
            c.y = y;
            c.visited = false;
            c.walls[0] = c.walls[1] = c.walls[2] = c.walls[3] = true;
        }
    }
}

void Maze::generate() {
    if (width <= 0 || height <= 0) {
        return;
    }

    std::vector<FrontierCell> frontier;
    frontier.reserve(width * height);

    // Start at (0, 0)
    grid[0][0].visited = true;

    auto addFrontier = [&](int x, int y) {
        if (!inBounds(x, y, width, height)) return;
        Cell& c = grid[y][x];
        if (!c.visited) {
            frontier.push_back({x, y});
        }
    };

    // Add neighbors of starting cell
    for (int dir = 0; dir < 4; ++dir) {
        int nx = 0 + DX[dir];
        int ny = 0 + DY[dir];
        addFrontier(nx, ny);
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    while (!frontier.empty()) {
        // Choose a random frontier cell
        std::uniform_int_distribution<std::size_t> dist(0, frontier.size() - 1);
        std::size_t idx = dist(gen);
        FrontierCell current = frontier[idx];

        // Remove from frontier
        frontier[idx] = frontier.back();
        frontier.pop_back();

        Cell& cell = grid[current.y][current.x];
        if (cell.visited) {
            continue;
        }

        // Find all visited neighbors
        std::vector<int> visitedDirs;
        visitedDirs.reserve(4);
        for (int dir = 0; dir < 4; ++dir) {
            int nx = current.x + DX[dir];
            int ny = current.y + DY[dir];
            if (inBounds(nx, ny, width, height) && grid[ny][nx].visited) {
                visitedDirs.push_back(dir);
            }
        }

        if (!visitedDirs.empty()) {
            // Connect to a random visited neighbor
            std::uniform_int_distribution<std::size_t> vd(0, visitedDirs.size() - 1);
            int dir = visitedDirs[vd(gen)];
            int nx = current.x + DX[dir];
            int ny = current.y + DY[dir];

            Cell& neighbor = grid[ny][nx];
            cell.walls[dir] = false;
            neighbor.walls[OPPOSITE[dir]] = false;
        }

        cell.visited = true;

        // Add this cell's unvisited neighbors to the frontier
        for (int dir = 0; dir < 4; ++dir) {
            int nx = current.x + DX[dir];
            int ny = current.y + DY[dir];
            addFrontier(nx, ny);
        }
    }
}

std::string Maze::toJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"width\":" << width << ",";
    oss << "\"height\":" << height << ",";
    oss << "\"grid\":[";

    for (int y = 0; y < height; ++y) {
        if (y > 0) {
            oss << ",";
        }
        oss << "[";
        for (int x = 0; x < width; ++x) {
            if (x > 0) {
                oss << ",";
            }
            const Cell& c = grid[y][x];
            oss << "{";
            oss << "\"x\":" << c.x << ",";
            oss << "\"y\":" << c.y << ",";
            oss << "\"walls\":[";
            for (int i = 0; i < 4; ++i) {
                if (i > 0) {
                    oss << ",";
                }
                oss << (c.walls[i] ? "true" : "false");
            }
            oss << "]";
            oss << "}";
        }
        oss << "]";
    }

    oss << "],";
    oss << "\"start\":{\"x\":0,\"y\":0},";
    oss << "\"end\":{\"x\":" << (width - 1) << ",\"y\":" << (height - 1) << "}";
    oss << "}";

    return oss.str();
}

