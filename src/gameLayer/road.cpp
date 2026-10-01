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

    // Outer solid road edge / shoulder stripes
    constexpr float shoulderWidth = 4.f;
    const gl2d::Color4f shoulderColor = { 0.92f, 0.92f, 0.96f, 0.85f };
    renderer.renderRectangle({ position.x, 0.f, shoulderWidth, static_cast<float>(windowHeight) }, shoulderColor);
    renderer.renderRectangle({ position.x + width - shoulderWidth, 0.f, shoulderWidth, static_cast<float>(windowHeight) }, shoulderColor);

    // Inner scrolling dashed lane divider markings
    constexpr float dashLength = 40.f;
    constexpr float dashGap = 40.f;
    constexpr float dashPeriod = dashLength + dashGap; // 80.f
    constexpr float dashWidth = 4.f;
    const gl2d::Color4f dashColor = { 0.96f, 0.94f, 0.82f, 0.85f }; // Clean arcade highway yellow-white

    float dashOffset = std::fmod(scrollOffset, dashPeriod);
    int dashRows = static_cast<int>(windowHeight / dashPeriod) + 2;

    for (int lane = 1; lane < NUM_LANES; ++lane)
    {
        float laneBoundaryX = getLaneLeft(lane) - dashWidth * 0.5f;
        for (int d = -1; d < dashRows; ++d)
        {
            float dashY = d * dashPeriod + dashOffset;
            renderer.renderRectangle(
                { laneBoundaryX, dashY, dashWidth, dashLength },
                dashColor);
        }
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
