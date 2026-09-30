#pragma once

#include <gl2d/gl2d.h>
#include <cstdint>

namespace UI
{
    // Main Menu: Title, Start prompt, Controls overview, Session high score
    void renderMainMenu(gl2d::Renderer2D& r, gl2d::Font font,
                        uint32_t highScore,
                        float screenWidth, float screenHeight);

    // Pause Overlay: "PAUSED" title, "PRESS P TO RESUME", current score, high score
    void renderPauseOverlay(gl2d::Renderer2D& r, gl2d::Font font,
                            uint32_t currentScore, uint32_t highScore,
                            float screenWidth, float screenHeight);

    // Game Over Screen: "GAME OVER" title, final score, high score, restart/menu options
    void renderGameOverScreen(gl2d::Renderer2D& r, gl2d::Font font,
                              uint32_t finalScore, uint32_t highScore,
                              float screenWidth, float screenHeight);
}
