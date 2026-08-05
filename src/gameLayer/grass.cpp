#include "grass.h"

#include <cmath>

void Grass::render(
    gl2d::Renderer2D& renderer,
    gl2d::Texture& texture,
    float roadLeft,
    float roadRight,
    int windowWidth,
    int windowHeight,
    float scrollOffset)
{
    float offset = std::fmod(scrollOffset, tileSize);

    int rows = static_cast<int>(windowHeight / tileSize) + 2;

    for (int y = -1; y < rows; y++)
    {
        float drawY = y * tileSize + offset;

        //-------------------------------------------------
        // Left Grass
        //-------------------------------------------------

        for (float x = 0; x < roadLeft; x += tileSize)
        {
            renderer.renderRectangle(
                {
                    x,
                    drawY,
                    tileSize,
                    tileSize
                },
                texture);
        }

        //-------------------------------------------------
        // Right Grass
        //-------------------------------------------------

        for (float x = roadRight; x < windowWidth; x += tileSize)
        {
            renderer.renderRectangle(
                {
                    x,
                    drawY,
                    tileSize,
                    tileSize
                },
                texture);
        }
    }
}