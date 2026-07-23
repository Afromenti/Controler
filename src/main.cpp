#include "core/Game.h"
#include "input/Controller.h"
#include "graphics/Renderer.h"
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
    Controller controller;
    
    if (!controller.connect())
    {
        cout << "Nie znaleziono pada \n";
        Sleep(1000);
        return 1;
    }

    Game game;
    Renderer renderer(game);
    renderer.start();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        float dt = elapsed.count();
        lastTime = currentTime;

        if (!game.logicState.gameover)
        {
            controller.processInput(game, dt);
            game.logic(dt);
        }
        else
        {
            controller.processInput(game, dt);
            if (game.logicState.shouldQuit)
            {
                break;
            }
        }
        
    }

    renderer.stop();
    return 0;
}
