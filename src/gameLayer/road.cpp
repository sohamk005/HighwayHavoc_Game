#include "road.h"

#include <cmath>

void Road::update(int windowWidth)
{
    position.x = (windowWidth - width) / 2.f;
}

void Road::render(gl2d::Renderer2D& renderer,
    gl2d::Texture& roadTexture,
    int windowHeight,
    float scrollOffset)
{
    float offset = std::fmod(scrollOffset, tileHeight);

    int rows = static_cast<int>(windowHeight / tileHeight) + 2;

    for (int i = -1; i < rows; i++)
    {
        renderer.renderRectangle(
            {
                position.x,
                position.y + i * tileHeight + offset,
                width,
                tileHeight
            },
            roadTexture);
    }
}

float Road::left() const
{
    return position.x;
}

float Road::right() const
{
    return position.x + width;
}