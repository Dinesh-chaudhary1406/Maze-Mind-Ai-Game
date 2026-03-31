cmake_minimum_required(VERSION 3.10)

project(dsa-game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE GAME_SERVER_SOURCES
    "src/*.cpp"
)

add_executable(game_server
    ${GAME_SERVER_SOURCES}
)

target_include_directories(game_server PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_package(Threads REQUIRED)
target_link_libraries(game_server PRIVATE Threads::Threads)

