#include "entities/Obstacle.h"

Obstacle::Obstacle() : x(-1.0f), y(-1.0f), speed(20.0f), active(false)
{
}

void Obstacle::spawn(float startX, float startY, float spd)
{
    x = startX;
    y = startY;
    speed = spd;
    active = true;
}

void Obstacle::update(float dt)
{
    if (active)
    {
        x -= speed * dt;
    }
}
