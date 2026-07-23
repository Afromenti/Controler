#pragma once
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

class Game;

class Controller
{
private:
    HANDLE handle;
    BYTE readBuffer[64];
    BYTE sharedBuffer[64];
    
    std::thread inputThread;
    std::atomic<bool> isRunning;
    std::mutex inputMutex;
    bool newDataAvailable;

    void readLoop();

public:
    Controller();
    ~Controller();
    bool connect();
    void disconnect();
    bool processInput(Game& game, float dt);
};
