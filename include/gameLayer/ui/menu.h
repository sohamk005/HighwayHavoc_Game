#pragma once

#include <gl2d/gl2d.h>
#include <cstdint>

struct VehicleEntry;

namespace UI
{
    // Main Menu: Title, Start prompt, Controls overview, Session high score, Wallet balance, Music setting
    void renderMainMenu(gl2d::Renderer2D& r, gl2d::Font font,
                        uint32_t highScore,
                        float screenWidth, float screenHeight,
                        uint32_t walletBalance = 0,
                        bool musicEnabled = true);

    // Pause Overlay: "PAUSED" title, "PRESS P TO RESUME", current score, high score, coins, Music setting
    void renderPauseOverlay(gl2d::Renderer2D& r, gl2d::Font font,
                            uint32_t currentScore, uint32_t highScore,
                            float screenWidth, float screenHeight,
                            uint32_t currentCoins = 0,
                            bool musicEnabled = true);

    // Game Over Screen: "GAME OVER" title, final score, high score, coins, wallet, restart/menu options
    void renderGameOverScreen(gl2d::Renderer2D& r, gl2d::Font font,
                              uint32_t finalScore, uint32_t highScore,
                              float screenWidth, float screenHeight,
                              uint32_t finalCoins = 0,
                              uint32_t walletBalance = 0);

    // Garage Screen (V2-M5): Vehicle preview, stats, status, buy/equip controls, wallet balance
    void renderGarageScreen(gl2d::Renderer2D& r, gl2d::Font font,
                            const VehicleEntry& vehicle,
                            bool isOwned, bool isSelected,
                            uint32_t currentWallet,
                            float screenWidth, float screenHeight,
                            size_t currentIndex, size_t totalVehicles,
                            bool showInsufficientCoins = false);
}
