#include "entities/ObstacleManager.h"
#include "core/GameState.h"
#include <cstdlib>
#include <cmath>

ObstacleManager::ObstacleManager(int width, int height) 
    : gameWidth(width), gameHeight(height), spawnTimer(0.0f), spawnCooldown(1.0f), maxObstacles(3)
{
    obstacles.resize(maxObstacles);
}

void ObstacleManager::reset()
{
    for (auto& obs : obstacles)
    {
        obs.active = false;
    }
    spawnTimer = 0.0f;
}

void ObstacleManager::update(float dt, GameState& state)
{
    spawnTimer += dt;
    if (spawnTimer >= spawnCooldown)
    {
        for (auto& obs : obstacles)
        {
            if (!obs.active)
            {
                float startY = static_cast<float>(rand() % gameHeight);
                float speed = 25.0f + static_cast<float>(rand() % 15); // Losowa predkosc 25 - 40
                obs.spawn(static_cast<float>(gameWidth - 1), startY, speed);
                spawnTimer = 0.0f;
                break; // spawn only one per interval
            }
        }
    }

    if (state.invincibilityTimer > 0.0f)
    {
        state.invincibilityTimer -= dt;
    }

    for (auto& obs : obstacles)
    {
        if (obs.active)
        {
            obs.update(dt);

            int dinoTileX = static_cast<int>(state.dinoX);
            int dinoTileY = static_cast<int>(state.dinoY);
            int obsTileX = static_cast<int>(obs.x);
            int obsTileY = static_cast<int>(obs.y);

            if (obsTileX == dinoTileX && obsTileY == dinoTileY)
            {
                if (state.invincibilityTimer <= 0.0f)
                {
                    state.lifes--;
                    state.invincibilityTimer = 2.0f; // Gracz jest niesmiertelny przez 2 sekundy po trafieniu
                }
            }

            if (obs.active && obs.x < 0.0f)
            {
                obs.active = false;
                state.score++;
            }
        }
    }
}
