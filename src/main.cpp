#include <cstdlib>
#include <iostream>
#include "crow_all.h"
#include "maze.h"
#include "pathfind.h"
#include "enemy.h"
#include <sstream>
#include <vector>
#include <fstream>

namespace {

void add_cors(crow::response& resp) {
    resp.set_header("Access-Control-Allow-Origin", "*");
    resp.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    resp.set_header("Access-Control-Allow-Headers", "Content-Type");
}

} // namespace

int main() {
    crow::SimpleApp app;

    // Root: serve frontend
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)([]() {
        std::ifstream file("frontend/index.html", std::ios::in | std::ios::binary);
        if (!file) {
            crow::response resp(500);
            add_cors(resp);
            resp.set_header("Content-Type", "text/plain");
            resp.body = "index.html not found";
            return resp;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        crow::response resp(200);
        add_cors(resp);
        resp.set_header("Content-Type", "text/html; charset=utf-8");
        resp.body = buffer.str();
        return resp;
    });

    // Preflight for root
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::OPTIONS)([]() {
        crow::response resp(200);
        add_cors(resp);
        return resp;
    });

    CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::GET)([]() {
        crow::response resp(200);
        crow::json::wvalue res;
        res["status"] = "ok";
        resp.code = 200;
        resp.set_header("Content-Type", "application/json");
         resp.body = res.dump();
        add_cors(resp);
        return resp;
    });

    CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::OPTIONS)([]() {
        crow::response resp(200);
        add_cors(resp);
        return resp;
    });

    CROW_ROUTE(app, "/api/generate-maze").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            if (!body.has("width") || !body.has("height")) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            int w = body["width"].i();
            int h = body["height"].i();
            if (w <= 0 || h <= 0) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"width and height must be positive"})";
                return badReq;
            }

            Maze maze(w, h);
            maze.generate();

            std::string json = maze.toJSON();
            crow::response resp(200);
            resp.set_header("Content-Type", "application/json");
            add_cors(resp);
            resp.body = std::move(json);
            return resp;
        });

    CROW_ROUTE(app, "/api/generate-maze").methods(crow::HTTPMethod::OPTIONS)([]() {
        crow::response resp(200);
        add_cors(resp);
        return resp;
    });

    CROW_ROUTE(app, "/api/pathfind").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            if (!body.has("grid") || !body.has("width") || !body.has("height") ||
                !body.has("start") || !body.has("end") || !body.has("algorithm")) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            int width = body["width"].i();
            int height = body["height"].i();
            auto startJson = body["start"];
            auto endJson = body["end"];
            int sx = startJson["x"].i();
            int sy = startJson["y"].i();
            int ex = endJson["x"].i();
            int ey = endJson["y"].i();

            if (width <= 0 || height <= 0) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"width and height must be positive"})";
                return badReq;
            }

            // Reconstruct grid of Cells from JSON.
            std::vector<std::vector<Cell>> grid;
            grid.resize(height, std::vector<Cell>(width));

            auto gridJson = body["grid"];
            for (int y = 0; y < height; ++y) {
                auto row = gridJson[y];
                for (int x = 0; x < width; ++x) {
                    auto cellJson = row[x];
                    Cell& c = grid[y][x];
                    c.x = x;
                    c.y = y;
                    c.visited = false;
                    auto wallsJson = cellJson["walls"];
                    for (int i = 0; i < 4; ++i) {
                        c.walls[i] = wallsJson[i].b();
                    }
                }
            }

            std::string algo = body["algorithm"].s();
            std::vector<std::pair<int, int>> visitedOrder;
            std::vector<std::pair<int, int>> path;

            if (algo == "dijkstra") {
                path = Pathfinder::dijkstra(grid, width, height, {sx, sy}, {ex, ey}, visitedOrder);
            } else if (algo == "astar") {
                path = Pathfinder::astar(grid, width, height, {sx, sy}, {ex, ey}, visitedOrder);
            } else {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"algorithm must be 'dijkstra' or 'astar'"})";
                return badReq;
            }

            bool found = !path.empty();

            std::ostringstream oss;
            oss << "{";
            oss << "\"algorithm\":\"" << algo << "\",";
            oss << "\"found\":" << (found ? "true" : "false") << ",";

            oss << "\"path\":[";
            for (std::size_t i = 0; i < path.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "{\"x\":" << path[i].first << ",\"y\":" << path[i].second << "}";
            }
            oss << "],";
            oss << "\"path_length\":" << path.size() << ",";

            oss << "\"visited_order\":[";
            for (std::size_t i = 0; i < visitedOrder.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "{\"x\":" << visitedOrder[i].first << ",\"y\":" << visitedOrder[i].second << "}";
            }
            oss << "]";
            oss << "}";

            crow::response resp(200);
            resp.set_header("Content-Type", "application/json");
            add_cors(resp);
            resp.body = oss.str();
            return resp;
        });

    CROW_ROUTE(app, "/api/pathfind").methods(crow::HTTPMethod::OPTIONS)([]() {
        crow::response resp(200);
        add_cors(resp);
        return resp;
    });

    CROW_ROUTE(app, "/api/enemy-move").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            if (!body.has("grid") || !body.has("width") || !body.has("height") ||
                !body.has("enemy") || !body.has("player")) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"invalid input"})";
                return badReq;
            }

            int width = body["width"].i();
            int height = body["height"].i();
            auto enemyJson = body["enemy"];
            auto playerJson = body["player"];
            int ex = enemyJson["x"].i();
            int ey = enemyJson["y"].i();
            int px = playerJson["x"].i();
            int py = playerJson["y"].i();

            if (width <= 0 || height <= 0) {
                crow::response badReq(400);
                add_cors(badReq);
                badReq.set_header("Content-Type", "application/json");
                badReq.body = R"({"error":"width and height must be positive"})";
                return badReq;
            }

            // Reconstruct grid of Cells from JSON.
            std::vector<std::vector<Cell>> grid;
            grid.resize(height, std::vector<Cell>(width));

            auto gridJson = body["grid"];
            for (int y = 0; y < height; ++y) {
                auto row = gridJson[y];
                for (int x = 0; x < width; ++x) {
                    auto cellJson = row[x];
                    Cell& c = grid[y][x];
                    c.x = x;
                    c.y = y;
                    c.visited = false;
                    auto wallsJson = cellJson["walls"];
                    for (int i = 0; i < 4; ++i) {
                        c.walls[i] = wallsJson[i].b();
                    }
                }
            }

            Enemy enemy(ex, ey);
            std::vector<std::pair<int, int>> fullPath;
            std::pair<int, int> next = enemy.nextMove(grid, width, height, px, py, fullPath);

            int distance = static_cast<int>(fullPath.size() > 0 ? fullPath.size() - 1 : 0);

            std::ostringstream oss;
            oss << "{";
            oss << "\"enemy_current\":{\"x\":" << ex << ",\"y\":" << ey << "},";
            oss << "\"enemy_next\":{\"x\":" << next.first << ",\"y\":" << next.second << "},";

            oss << "\"path_to_player\":[";
            for (std::size_t i = 0; i < fullPath.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "{\"x\":" << fullPath[i].first << ",\"y\":" << fullPath[i].second << "}";
            }
            oss << "],";

            oss << "\"distance\":" << distance;
            oss << "}";

            crow::response resp(200);
            resp.set_header("Content-Type", "application/json");
            add_cors(resp);
            resp.body = oss.str();
            return resp;
        });

    CROW_ROUTE(app, "/api/enemy-move").methods(crow::HTTPMethod::OPTIONS)([]() {
        crow::response resp(200);
        add_cors(resp);
        return resp;
    });

    uint16_t port = 8080;

    const char* port_env = std::getenv("PORT");
    if(port_env) {
        port = static_cast<uint16_t>(std::stoi(port_env));
    }
    
    std::cout << "Server running on port " << port << std::endl;
    
    app.port(port).multithreaded().run();
    return 0;
}

