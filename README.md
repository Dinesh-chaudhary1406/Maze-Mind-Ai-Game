# DSA Pathfinding Game

An interactive maze and pathfinding game built with C++ (Crow web framework) and an HTML5 Canvas frontend. It showcases classic graph algorithms such as Prim's maze generation, Dijkstra's algorithm, A\*, and BFS-based enemy AI.

## Algorithms Used

- **Prim's Algorithm (Maze Generation)**  
  Used to carve a perfect maze (no loops, fully connected) from an initially walled grid.

- **Dijkstra's Algorithm (Shortest Path)**  
  Computes the shortest path in the maze using uniform edge costs and no heuristic.

- **A\* Search (Heuristic Shortest Path)**  
  Uses Manhattan distance as a heuristic to guide search, usually exploring fewer cells than Dijkstra in this grid setting.

- **Breadth-First Search (Enemy AI)**  
  The enemy uses BFS to find the shortest path to the player and moves one step toward them each turn.

## Running Locally with CMake

### Prerequisites

- A C++17-capable compiler (e.g. `g++` or `clang++`)
- CMake (3.10+)
- POSIX threads support (typically available by default)
- `crow_all.h` downloaded into the `include/` directory

### Steps

1. **Clone / open the project**  
   Ensure your directory looks like:

   - `CMakeLists.txt`
   - `include/crow_all.h`
   - `src/` (C++ sources)
   - `frontend/index.html`

2. **Generate build files**

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build the server**

   ```bash
   cmake --build build --parallel
   ```

4. **Run the server**

   ```bash
   ./build/game_server
   ```

5. **Open the game**  
   Visit `http://localhost:8080` in your browser.  
   The C++ server serves `frontend/index.html` at the root path and exposes the JSON APIs on the same port.

## Running with Docker

The repo includes a production-focused `Dockerfile` and `docker-compose.yml`.

### Build and run with Docker only

```bash
docker build -t dsa-game .
docker run --rm -p 8080:8080 dsa-game
```

Then open `http://localhost:8080` in your browser.

### Using docker-compose

```bash
docker-compose up --build
```

This:

- Builds the image from the provided `Dockerfile`
- Runs the `game_server` service
- Publishes port `8080` on the host
- Mounts the local `frontend/` directory into `/app/frontend` inside the container, so frontend changes are picked up without rebuilding

Open `http://localhost:8080` to play.

## Deploying to Render

You can deploy this project to [Render](https://render.com) using the existing Dockerfile.

1. **Push the repository to GitHub/GitLab**.
2. In Render, create a **new Web Service**:
   - **Environment**: Docker
   - **Repository**: your repo
   - **Dockerfile path**: `Dockerfile`
3. **Port configuration**:
   - Set the Render service port to **8080** (the container listens on 8080).
4. **Start command**:
   - The Dockerfile already specifies `CMD ["./build/game_server"]`, so no custom start command is required.

Once Render finishes building and starts the service, open the Render URL in your browser; the game will be available at the root path (`/`) and all API endpoints share the same origin.

