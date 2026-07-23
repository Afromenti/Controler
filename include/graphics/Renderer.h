#pragma once
#include "core/Game.h"
#include <thread>
#include <atomic>
#include <vector>
#include <cmath>

struct FrameTimeBucket
{
    float accumulatedTimeMs = 0.0f;
    int frameCount = 0;
    int currentFps = 60;
    std::vector<float> frameTimesMs;

    void addFrame(float dtSeconds)
    {
        float dtMs = dtSeconds * 1000.0f;
        if (dtMs < 0.001f) dtMs = 0.001f;
        accumulatedTimeMs += dtMs;
        frameCount++;
        frameTimesMs.push_back(dtMs);

        if (accumulatedTimeMs >= 1000.0f)
        {
            float excessMs = accumulatedTimeMs - 1000.0f;
            float avgDtMs = accumulatedTimeMs / frameCount;
            if (avgDtMs > 0.0f)
            {
                currentFps = static_cast<int>(std::round(1000.0f / avgDtMs));
            }

            accumulatedTimeMs = excessMs;
            frameCount = 0;
            frameTimesMs.clear();
        }
    }
};

class Renderer
{
private:
    Game& game;
    std::atomic<bool> isRunning;
    std::thread renderThread;

    FrameTimeBucket fpsBucket;
    bool wasOver;

    void drawLoop();

public:
    Renderer(Game& g);
    ~Renderer();
    
    void start();
    void stop();
};
