#include "enemy.h"

#include <queue>

namespace {

struct BFSNode {
    int x;
    int y;
};

bool inBounds(int x, int y, int w, int h) {
    return x >= 0 && x < w && y >= 0 && y < h;
}

} // namespace

std::pair<int, int> Enemy::nextMove(
    const std::vector<std::vector<Cell>>& grid,
    int width,
    int height,
    int player_x,
    int player_y,
    std::vector<std::pair<int, int>>& fullPath) {

    fullPath.clear();

    if (!inBounds(x, y, width, height) || !inBounds(player_x, player_y, width, height)) {
        return {x, y};
    }

    if (x == player_x && y == player_y) {
        fullPath.emplace_back(x, y);
        return {x, y};
    }

    std::queue<BFSNode> q;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<std::pair<int, int>>> parent(
        height, std::vector<std::pair<int, int>>(width, {-1, -1}));

    q.push({x, y});
    visited[y][x] = true;

    // Directions: 0 = top, 1 = right, 2 = bottom, 3 = left
    const int DX[4] = {0, 1, 0, -1};
    const int DY[4] = {-1, 0, 1, 0};

    bool found = false;

    while (!q.empty()) {
        BFSNode cur = q.front();
        q.pop();

        if (cur.x == player_x && cur.y == player_y) {
            found = true;
            break;
        }

        const Cell& c = grid[cur.y][cur.x];

        for (int dir = 0; dir < 4; ++dir) {
            int nx = cur.x + DX[dir];
            int ny = cur.y + DY[dir];

            if (!inBounds(nx, ny, width, height)) {
                continue;
            }

            // Can move only through open walls (false).
            if (c.walls[dir]) {
                continue;
            }

            if (!visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = {cur.x, cur.y};
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        // No path to player; stay in place.
        fullPath.emplace_back(x, y);
        return {x, y};
    }

    // Reconstruct full path from player back to enemy.
    int cx = player_x;
    int cy = player_y;
    while (!(cx == x && cy == y)) {
        fullPath.emplace_back(cx, cy);
        auto p = parent[cy][cx];
        if (p.first == -1 && p.second == -1) {
            break;
        }
        cx = p.first;
        cy = p.second;
    }
    fullPath.emplace_back(x, y);

    std::reverse(fullPath.begin(), fullPath.end());

    if (fullPath.size() < 2) {
        return {x, y};
    }

    // Next move is the second cell in the path.
    return fullPath[1];
}

