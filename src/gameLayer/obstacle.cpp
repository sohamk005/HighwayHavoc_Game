#include "obstacle.h"

void Obstacle::update(float scrollSpeed, float deltaTime)
{
    position.y += scrollSpeed * deltaTime;
}

void Obstacle::render(gl2d::Renderer2D& renderer)
{
    if (!active || texture == nullptr)
        return;

    renderer.renderRectangle(
        {
            position,
            size
        },
        *texture);
}