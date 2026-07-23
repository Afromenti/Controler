#pragma once
#include <vector>
#include <utility>

struct GameState
{
    bool gameover;
    bool shouldQuit;
    int score;
    int lifes;
    float dinoX;
    float dinoY;
    float invincibilityTimer;
    std::vector<std::pair<int, int>> activeObstacles;
};
