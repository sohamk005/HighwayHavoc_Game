#pragma once

#include <gl2d/gl2d.h>
#include <cstdint>

namespace UI
{
    // Renders the in-game HUD:
    // - Health bar with "HEALTH" label and dynamic green/yellow/red color transitions
    // - Framed Score Panel with "SCORE: <current>" and "BEST: <high>"
    void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                          uint32_t currentScore, uint32_t highScore,
                          int health, int maxHealth,
                          float screenWidth, float screenHeight);

    // Standalone health bar renderer for testing or isolated rendering
    void renderHealthBar(gl2d::Renderer2D& r, gl2d::Font font,
                         int health, int maxHealth,
                         float x, float y, float width, float height);
}
