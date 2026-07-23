#pragma once

class Obstacle
{
public:
    float x, y;
    float speed;
    bool active;

    Obstacle();
    void spawn(float startX, float startY, float spd);
    void update(float dt);
};
