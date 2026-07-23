#include "core/Game.h"
#include <cstdlib>

Game::Game() : obstacleManager(GAME_WIDTH, GAME_HEIGHT)
{
    setup();
    logicState.shouldQuit = false;
    renderState = logicState;
}

void Game::setup()
{
    logicState.gameover = false;
    logicState.score = 0;
    logicState.dinoX = GAME_WIDTH / 2.0f; 
    logicState.dinoY = GAME_HEIGHT / 2.0f;
    logicState.lifes = 3;
    logicState.invincibilityTimer = 0.0f;
    logicState.activeObstacles.clear();
    obstacleManager.reset();

    std::lock_guard<std::mutex> lock(stateMutex);
    renderState = logicState;
}

void Game::logic(float dt)
{
    if (logicState.gameover) 
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        renderState = logicState;
        return;
    }

    obstacleManager.update(dt, logicState);

    if (logicState.lifes <= 0)
    {
        logicState.gameover = true;
    }

    if (logicState.dinoX >= static_cast<float>(GAME_WIDTH) || logicState.dinoY >= static_cast<float>(GAME_HEIGHT) || logicState.dinoX < 0.0f || logicState.dinoY < 0.0f)
    {
        logicState.lifes = 0;
        logicState.gameover = true;
    }

    logicState.activeObstacles.clear();
    for (const auto& obs : obstacleManager.obstacles)
    {
        if (obs.active)
        {
            logicState.activeObstacles.push_back({static_cast<int>(obs.x), static_cast<int>(obs.y)});
        }
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    renderState = logicState;
}

void Game::getRenderFrame(RenderFrame& outFrame, int fps)
{
    GameState state;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        state = renderState;
    }

    if (state.gameover)
    {
        auto printPadded = [](const std::string& text) -> std::string {
            int padding = (GAME_WIDTH + 2) - static_cast<int>(text.length());
            if (padding < 0) padding = 0;
            return text + std::string(padding, ' ') + "\n";
        };

        std::string res;
        res += printPadded("Twoj ostatni wynik to: " + std::to_string(state.score));
        res += printPadded("Nacisnij [START], zeby wznowic gre");
        res += printPadded("Nacisnij [B], zeby zakonczyc program");
        
        std::string emptyRow(GAME_WIDTH + 2, ' ');
        for (int i = 0; i < GAME_HEIGHT + 1; i++) 
        {
            res += emptyRow + "\n";
        }
        outFrame.text = res;
    }
    else
    {
        std::string horizontalBorder(GAME_WIDTH + 2, '*');
        std::string frame;
        frame += horizontalBorder + '\n';

        std::vector<std::string> grid(GAME_HEIGHT, std::string(GAME_WIDTH + 2, ' '));
        for (int i = 0; i < GAME_HEIGHT; i++)
        {
            grid[i][0] = '*';
            grid[i][GAME_WIDTH + 1] = '*';
        }

        int dX = static_cast<int>(state.dinoX);
        int dY = static_cast<int>(state.dinoY);
        if (dY >= 0 && dY < GAME_HEIGHT && dX >= 0 && dX < GAME_WIDTH)
        {
            if (state.invincibilityTimer <= 0.0f || static_cast<int>(state.invincibilityTimer * 10) % 2 == 0)
            {
                grid[dY][dX + 1] = 'D'; 
            }
        }

        for (const auto& obsPos : state.activeObstacles)
        {
            if (obsPos.second >= 0 && obsPos.second < GAME_HEIGHT && obsPos.first >= 0 && obsPos.first < GAME_WIDTH)
            {
                grid[obsPos.second][obsPos.first + 1] = 'X';
            }
        }

        for (const auto& row : grid)
        {
            frame += row + '\n';
        }

        frame += horizontalBorder + '\n';
        frame += "Score: " + std::to_string(state.score) + " Lives: " + std::to_string(state.lifes) + " FPS: " + std::to_string(fps) + "       \n";
        frame += "Nacisnij [B] zeby wyjsc z gry.                         \n";

        outFrame.text = frame;
    }
}
