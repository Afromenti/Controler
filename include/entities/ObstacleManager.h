#pragma once
#include "entities/Obstacle.h"
#include <vector>

struct GameState;

class ObstacleManager
{
private:
    float spawnTimer;
    float spawnCooldown;
    int maxObstacles;
    
public:
    std::vector<Obstacle> obstacles;
    int gameWidth;
    int gameHeight;

    ObstacleManager(int width, int height);
    void update(float dt, GameState& state);
    void reset();
};
