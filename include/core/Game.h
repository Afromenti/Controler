#pragma once
#include <mutex>
#include "entities/ObstacleManager.h"
#include "core/GameState.h"
#include "graphics/RenderFrame.h"

const int GAME_WIDTH = 200;
const int GAME_HEIGHT = 50;

class Game
{
public:
    GameState logicState;
    GameState renderState;
    
    ObstacleManager obstacleManager;
    std::mutex stateMutex;

    Game();
    void setup();
    void logic(float dt);
    void getRenderFrame(RenderFrame& outFrame, int fps);
};
