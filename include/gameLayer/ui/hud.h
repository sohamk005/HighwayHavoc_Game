#pragma once

#include <gl2d/gl2d.h>
#include <cstdint>

namespace UI
{
    // Renders the in-game HUD:
    // - Health bar with "HEALTH" label and dynamic green/yellow/red color transitions
    // - Framed Score Panel with "SCORE: <current>" and "BEST: <high>"
    // - Framed Coin Counter Panel with "COINS  <count>"
    // - Framed Power-Up Status Panel (when Shield or 2X Coin Multiplier active)
    void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                          uint32_t currentScore, uint32_t highScore,
                          int health, int maxHealth,
                          uint32_t coinCount,
                          bool hasShield, float shieldTimer,
                          bool isMultiplierActive, float multiplierTimer,
                          float screenWidth, float screenHeight);

    inline void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                                 uint32_t currentScore, uint32_t highScore,
                                 int health, int maxHealth,
                                 uint32_t coinCount,
                                 float screenWidth, float screenHeight)
    {
        renderPlayingHUD(r, font, currentScore, highScore, health, maxHealth, coinCount,
                         false, 0.f, false, 0.f, screenWidth, screenHeight);
    }

    inline void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                                 uint32_t currentScore, uint32_t highScore,
                                 int health, int maxHealth,
                                 float screenWidth, float screenHeight)
    {
        renderPlayingHUD(r, font, currentScore, highScore, health, maxHealth, 0,
                         false, 0.f, false, 0.f, screenWidth, screenHeight);
    }


    // Standalone health bar renderer for testing or isolated rendering
    void renderHealthBar(gl2d::Renderer2D& r, gl2d::Font font,
                         int health, int maxHealth,
                         float x, float y, float width, float height);
}
