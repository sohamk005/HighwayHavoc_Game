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

float Road::grassLeftWidth() const
{
    return position.x;
}

float Road::grassRightStart() const
{
    return position.x + width;
}

float Road::getLaneWidth() const
{
    return width / static_cast<float>(NUM_LANES);
}

float Road::getLaneLeft(int laneIndex) const
{
    return position.x + static_cast<float>(laneIndex) * getLaneWidth();
}

float Road::getLaneRight(int laneIndex) const
{
    return position.x + static_cast<float>(laneIndex + 1) * getLaneWidth();
}

float Road::getLaneCenter(int laneIndex) const
{
    return position.x + (static_cast<float>(laneIndex) + 0.5f) * getLaneWidth();
}

int Road::getLaneIndexFromX(float x) const
{
    float relativeX = x - position.x;
    int index = static_cast<int>(relativeX / getLaneWidth());
    if (index < 0) return 0;
    if (index >= NUM_LANES) return NUM_LANES - 1;
    return index;
}
