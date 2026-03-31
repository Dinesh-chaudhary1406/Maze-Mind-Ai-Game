#include "pathfind.h"
#include <algorithm>
#include <queue>
#include <cmath>
#include <limits>

namespace {

struct PQNode {
    int x;
    int y;
    float g_cost;
    float h_cost;
    float f_cost;
};

struct ComparePQNode {
    bool operator()(const PQNode& a, const PQNode& b) const {
        return a.f_cost > b.f_cost;
    }
};

float manhattan(int x1, int y1, int x2, int y2) {
    return static_cast<float>(std::abs(x1 - x2) + std::abs(y1 - y2));
}

bool inBounds(int x, int y, int w, int h) {
    return x >= 0 && x < w && y >= 0 && y < h;
}

} // namespace

std::vector<std::pair<int, int>> Pathfinder::getNeighbors(
    const std::vector<std::vector<Cell>>& grid,
    int width,
    int height,
    int x,
    int y) {
    std::vector<std::pair<int, int>> neighbors;

    if (!inBounds(x, y, width, height)) {
        return neighbors;
    }

    const Cell& c = grid[y][x];

    // 0 = top, 1 = right, 2 = bottom, 3 = left
    // Check each direction; neighbor is valid if wall is open (false).
    if (!c.walls[0] && inBounds(x, y - 1, width, height)) {
        neighbors.emplace_back(x, y - 1);
    }
    if (!c.walls[1] && inBounds(x + 1, y, width, height)) {
        neighbors.emplace_back(x + 1, y);
    }
    if (!c.walls[2] && inBounds(x, y + 1, width, height)) {
        neighbors.emplace_back(x, y + 1);
    }
    if (!c.walls[3] && inBounds(x - 1, y, width, height)) {
        neighbors.emplace_back(x - 1, y);
    }

    return neighbors;
}

static std::vector<std::pair<int, int>> reconstructPath(
    const std::vector<std::vector<Node>>& cameFrom,
    int sx,
    int sy,
    int ex,
    int ey) {
    std::vector<std::pair<int, int>> path;

    if (ex < 0 || ey < 0) {
        return path;
    }

    int cx = ex;
    int cy = ey;
    while (!(cx == sx && cy == sy)) {
        path.emplace_back(cx, cy);
        const Node& n = cameFrom[cy][cx];
        if (!n.parent) {
            break;
        }
        cx = n.parent->x;
        cy = n.parent->y;
    }
    path.emplace_back(sx, sy);
    std::reverse(path.begin(), path.end());
    return path;
}

static std::vector<std::pair<int, int>> runSearch(
    const std::vector<std::vector<Cell>>& grid,
    int width,
    int height,
    std::pair<int, int> start,
    std::pair<int, int> end,
    bool useHeuristic,
    std::vector<std::pair<int, int>>& visitedOrder) {

    visitedOrder.clear();

    if (!inBounds(start.first, start.second, width, height) ||
        !inBounds(end.first, end.second, width, height)) {
        return {};
    }

    std::vector<std::vector<float>> gScore(height, std::vector<float>(width, std::numeric_limits<float>::infinity()));
    std::vector<std::vector<float>> hScore(height, std::vector<float>(width, 0.0f));
    std::vector<std::vector<Node>> parents(height, std::vector<Node>(width));
    std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            parents[y][x].x = x;
            parents[y][x].y = y;
            parents[y][x].g_cost = std::numeric_limits<float>::infinity();
            parents[y][x].h_cost = 0.0f;
            parents[y][x].f_cost = std::numeric_limits<float>::infinity();
            parents[y][x].parent = nullptr;
        }
    }

    std::priority_queue<PQNode, std::vector<PQNode>, ComparePQNode> openSet;

    int sx = start.first;
    int sy = start.second;
    int ex = end.first;
    int ey = end.second;

    gScore[sy][sx] = 0.0f;
    hScore[sy][sx] = useHeuristic ? manhattan(sx, sy, ex, ey) : 0.0f;
    float fStart = gScore[sy][sx] + hScore[sy][sx];

    parents[sy][sx].g_cost = gScore[sy][sx];
    parents[sy][sx].h_cost = hScore[sy][sx];
    parents[sy][sx].f_cost = fStart;
    parents[sy][sx].parent = nullptr;

    openSet.push({sx, sy, gScore[sy][sx], hScore[sy][sx], fStart});

    while (!openSet.empty()) {
        PQNode current = openSet.top();
        openSet.pop();

        if (closed[current.y][current.x]) {
            continue;
        }
        closed[current.y][current.x] = true;

        visitedOrder.emplace_back(current.x, current.y);

        if (current.x == ex && current.y == ey) {
            return reconstructPath(parents, sx, sy, ex, ey);
        }

        auto neighbors = Pathfinder::getNeighbors(grid, width, height, current.x, current.y);
        for (const auto& nb : neighbors) {
            int nx = nb.first;
            int ny = nb.second;

            if (closed[ny][nx]) {
                continue;
            }

            float tentativeG = gScore[current.y][current.x] + 1.0f; // uniform cost

            if (tentativeG < gScore[ny][nx]) {
                gScore[ny][nx] = tentativeG;
                float h = useHeuristic ? manhattan(nx, ny, ex, ey) : 0.0f;
                float f = tentativeG + h;

                parents[ny][nx].g_cost = tentativeG;
                parents[ny][nx].h_cost = h;
                parents[ny][nx].f_cost = f;
                parents[ny][nx].parent = &parents[current.y][current.x];

                openSet.push({nx, ny, tentativeG, h, f});
            }
        }
    }

    return {};
}

std::vector<std::pair<int, int>> Pathfinder::dijkstra(
    const std::vector<std::vector<Cell>>& grid,
    int width,
    int height,
    std::pair<int, int> start,
    std::pair<int, int> end,
    std::vector<std::pair<int, int>>& visitedOrder) {
    return runSearch(grid, width, height, start, end, false, visitedOrder);
}

std::vector<std::pair<int, int>> Pathfinder::astar(
    const std::vector<std::vector<Cell>>& grid,
    int width,
    int height,
    std::pair<int, int> start,
    std::pair<int, int> end,
    std::vector<std::pair<int, int>>& visitedOrder) {
    return runSearch(grid, width, height, start, end, true, visitedOrder);
}

