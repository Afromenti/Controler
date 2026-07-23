#include "graphics/Renderer.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

void hideCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void resizeWindow()
{
    HWND console = GetConsoleWindow();
    if (console != NULL)
    {
        MoveWindow(console, 0, 0, 1920, 1080, TRUE);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD size = { (SHORT)(GAME_WIDTH + 5), (SHORT)(GAME_HEIGHT + 5) }; 
        SetConsoleScreenBufferSize(hOut, size);
    }
}

Renderer::Renderer(Game& g) : game(g), isRunning(false), wasOver(false)
{
    resizeWindow();
    hideCursor();
}

Renderer::~Renderer()
{
    stop();
}

void Renderer::start()
{
    if (!isRunning)
    {
        isRunning = true;
        renderThread = std::thread(&Renderer::drawLoop, this);
    }
}

void Renderer::stop()
{
    isRunning = false;
    if (renderThread.joinable())
    {
        renderThread.join();
    }
}

void Renderer::drawLoop()
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> elapsed = frameStart - lastTime;
        lastTime = frameStart;
        float dt = elapsed.count();

        fpsBucket.addFrame(dt);

        RenderFrame frame;
        game.getRenderFrame(frame, fpsBucket.currentFps);

        setCursorPosition(0, 0);
        DWORD written = 0;
        WriteConsoleA(hOut, frame.text.c_str(), static_cast<DWORD>(frame.text.length()), &written, NULL);
    }
}
