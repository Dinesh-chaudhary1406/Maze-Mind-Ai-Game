# Maze-Mind-Ai-Game
MazeMind AI – Interactive pathfinding visualization game built with C++ (Crow), Docker, and JavaScript. Implements Dijkstra, A, and BFS with real-time REST APIs.*


# MazeMind AI 🧠🧩

An interactive maze escape game and pathfinding visualizer built using a C++ backend and a JavaScript frontend. The project demonstrates how classic graph algorithms like Dijkstra, A*, and BFS work in real-time through an engaging game interface.

## Live Demo
(Deploy link here after deployment)

## Features

Interactive maze generation  
Real-time pathfinding visualization  
Enemy AI using BFS shortest path  
REST API backend using Crow (C++)  
Dockerized deployment  
HTML5 Canvas game frontend  

## Algorithms Used

### Maze Generation
Prim's Algorithm (Minimum Spanning Tree)

### Pathfinding Algorithms
Dijkstra Algorithm → Safe shortest path  
A* Algorithm → Optimized shortest path  

### Enemy AI
Breadth First Search (BFS)

## Tech Stack

Backend:
C++
Crow HTTP Framework
CMake
Boost
ASIO

Frontend:
HTML5 Canvas
Vanilla JavaScript
CSS

DevOps:
Docker
GitHub
(Render/AWS deployment)

## Project Structure
dsa-game/
│
├── src/
│ ├── main.cpp
│ ├── maze.cpp
│ ├── pathfind.cpp
│ ├── enemy.cpp
│
├── include/
│
├── frontend/
│ └── index.html
│
├── Dockerfile
├── docker-compose.yml
└── CMakeLists.txt


## How to Run Locally

### Using Docker (recommended)

Build image:


docker build -t mazemind-ai .


Run container:


docker run -p 8080:8080 mazemind-ai


Open:


http://localhost:8080


---

### Without Docker

Install dependencies:


cmake
g++
boost
asio


Build:


cmake -B build
cmake --build build
