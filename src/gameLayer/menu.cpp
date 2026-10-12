#include "ui/menu.h"
#include "ui/uiTheme.h"
#include "garage.h"
#include <string>

namespace UI
{

void renderMainMenu(gl2d::Renderer2D& r, gl2d::Font font,
                    uint32_t highScore,
                    float screenWidth, float screenHeight,
                    uint32_t walletBalance,
                    bool musicEnabled)
{
    // Fullscreen backdrop dim
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight }, UITheme::BackdropDim);

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;

    // Game Title
    r.renderText({ cx, screenHeight * 0.18f }, "HIGHWAY HAVOC", font,
                 UITheme::TextAccent, UITheme::ScaleTitle,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Subtitle
    r.renderText({ cx, screenHeight * 0.25f }, "RETRO ARCADE SPEEDWAY", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Call to Action
    r.renderText({ cx, screenHeight * 0.35f }, "PRESS ENTER OR SPACE TO START", font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Controls Card (expanded height to 170.f to accommodate M toggle)
    constexpr float cardW = 390.f;
    constexpr float cardH = 190.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = screenHeight * 0.40f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 1.5f);

    r.renderText({ cx, cardY + 18.f }, "CONTROLS", font,
                 UITheme::TextAccent, UITheme::ScaleSmall,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Subtle divider line
    r.renderRectangle({ cardX + 32.f, cardY + 34.f, cardW - 64.f, 1.f }, UITheme::PanelBorder);

    const char* controls[] = {
        "W / S    -   MOVE UP / DOWN",
        "A / D    -   STEER LEFT / RIGHT",
        "P        -   PAUSE GAME",
        "R        -   QUICK RESTART",
        "M        -   TOGGLE MUSIC",
        "G        -   GARAGE & VEHICLES"
    };

    const float lineSpacing = 22.f;
    const float startLineY = cardY + 48.f;
    for (size_t i = 0; i < 6; ++i)
    {
        glm::vec2 sz = r.getTextSize(controls[i], font, UITheme::ScaleSmall);
        float textCenterX = cardX + 36.f + sz.x * 0.5f;
        float textCenterY = startLineY + static_cast<float>(i) * lineSpacing;

        r.renderText({ textCenterX, textCenterY }, controls[i], font,
                     UITheme::TextSecondary, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }

    // Music Status Indicator
    std::string musicStr = std::string("MUSIC: ") + (musicEnabled ? "ON" : "OFF");
    gl2d::Color4f musicColor = musicEnabled ? UITheme::TextSuccess : UITheme::TextSecondary;
    r.renderText({ cx, screenHeight * 0.70f }, musicStr.c_str(), font,
                 musicColor, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);

    // High Score Banner
    std::string hsStr = "ALL-TIME HIGH SCORE: " + std::to_string(highScore);
    r.renderText({ cx, screenHeight * 0.745f }, hsStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Wallet Total Coins Banner (V2-M2)
    std::string walletStr = "TOTAL COINS: " + std::to_string(walletBalance);
    r.renderText({ cx, screenHeight * 0.79f }, walletStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);
}

void renderPauseOverlay(gl2d::Renderer2D& r, gl2d::Font font,
                        uint32_t currentScore, uint32_t highScore,
                        float screenWidth, float screenHeight,
                        uint32_t currentCoins,
                        bool musicEnabled)
{
    // Fullscreen backdrop dim
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight }, UITheme::BackdropDim);

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;
    const float cy = screenHeight * 0.5f;

    // Centered modal card
    constexpr float cardW = 400.f;
    constexpr float cardH = 290.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = cy - cardH * 0.5f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 2.0f);

    // Title
    r.renderText({ cx, cardY + 32.f }, "PAUSED", font,
                 UITheme::TextAccent, UITheme::ScaleHeader,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Divider line
    r.renderRectangle({ cardX + 36.f, cardY + 54.f, cardW - 72.f, 1.f }, UITheme::PanelBorder);

    // Resume prompt
    r.renderText({ cx, cardY + 76.f }, "PRESS P TO RESUME", font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Current Score
    std::string scoreStr = "CURRENT SCORE: " + std::to_string(currentScore);
    r.renderText({ cx, cardY + 112.f }, scoreStr.c_str(), font,
                 UITheme::TextPrimary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Coins
    std::string coinStr = "COINS:         " + std::to_string(currentCoins);
    r.renderText({ cx, cardY + 138.f }, coinStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // High Score
    std::string highStr = "HIGH SCORE:    " + std::to_string(highScore);
    r.renderText({ cx, cardY + 164.f }, highStr.c_str(), font,
                 UITheme::TextSecondary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Music Status Line
    std::string musicStr = std::string("MUSIC: ") + (musicEnabled ? "ON" : "OFF") + " (PRESS M)";
    gl2d::Color4f musicColor = musicEnabled ? UITheme::TextSuccess : UITheme::TextSecondary;
    r.renderText({ cx, cardY + 204.f }, musicStr.c_str(), font,
                 musicColor, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Quick restart & menu hints
    r.renderText({ cx, cardY + 236.f }, "PRESS R TO RESTART", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);

    r.renderText({ cx, cardY + 260.f }, "PRESS ESC TO MAIN MENU", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);
}

void renderGameOverScreen(gl2d::Renderer2D& r, gl2d::Font font,
                          uint32_t finalScore, uint32_t highScore,
                          float screenWidth, float screenHeight,
                          uint32_t finalCoins,
                          uint32_t walletBalance)
{
    // Fullscreen backdrop dim with dark red undertone
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight },
                      gl2d::Color4f{ 0.12f, 0.03f, 0.03f, 0.78f });

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;
    const float cy = screenHeight * 0.5f;

    // Centered modal card with crimson border
    constexpr float cardW = 440.f;
    constexpr float cardH = 336.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = cy - cardH * 0.5f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorderDanger, 2.0f);

    // Title
    r.renderText({ cx, cardY + 36.f }, "GAME OVER", font,
                 UITheme::TextDanger, UITheme::ScaleHeader,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Crimson divider line
    r.renderRectangle({ cardX + 36.f, cardY + 58.f, cardW - 72.f, 1.f }, UITheme::PanelBorderDanger);

    float textOffsetY = cardY + 80.f;
    // High score notification if achieved
    if (finalScore >= highScore && finalScore > 0)
    {
        r.renderText({ cx, textOffsetY }, "★  NEW HIGH SCORE!  ★", font,
                     UITheme::TextAccent, UITheme::ScaleSmall,
                     4.f, 3.f, true, UITheme::TextShadow);
        textOffsetY += 26.f;
    }
    else
    {
        textOffsetY += 10.f;
    }

    // Final Score
    std::string scoreStr = "FINAL SCORE: " + std::to_string(finalScore);
    r.renderText({ cx, textOffsetY }, scoreStr.c_str(), font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);
    textOffsetY += 30.f;

    // Coins Collected in Current Run
    std::string coinsStr = "RUN COINS:   " + std::to_string(finalCoins);
    r.renderText({ cx, textOffsetY }, coinsStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);
    textOffsetY += 26.f;

    // Total Banked Wallet Balance
    std::string totalStr = "TOTAL COINS: " + std::to_string(walletBalance);
    r.renderText({ cx, textOffsetY }, totalStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);
    textOffsetY += 26.f;

    // High Score
    std::string highStr = "HIGH SCORE:  " + std::to_string(highScore);
    r.renderText({ cx, textOffsetY }, highStr.c_str(), font,
                 UITheme::TextSecondary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Action Prompts
    r.renderText({ cx, cardY + 260.f }, "PRESS R TO RESTART", font,
                 UITheme::TextSuccess, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);

    r.renderText({ cx, cardY + 296.f }, "PRESS ESC TO RETURN TO MAIN MENU", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);
}

void renderGarageScreen(gl2d::Renderer2D& r, gl2d::Font font,
                        const VehicleEntry& vehicle,
                        bool isOwned, bool isSelected,
                        uint32_t currentWallet,
                        float screenWidth, float screenHeight,
                        size_t currentIndex, size_t totalVehicles,
                        bool showInsufficientCoins)
{
    // Fullscreen backdrop dim
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight }, UITheme::BackdropDim);

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;

    // Header Title
    r.renderText({ cx, screenHeight * 0.12f }, "GARAGE & SHOWROOM", font,
                 UITheme::TextAccent, UITheme::ScaleTitle,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Wallet Balance Banner
    std::string walletStr = "WALLET: " + std::to_string(currentWallet) + " COINS";
    r.renderText({ cx, screenHeight * 0.19f }, walletStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Center Showcase Card
    constexpr float cardW = 480.f;
    constexpr float cardH = 370.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = screenHeight * 0.24f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 2.0f);

    // Carousel Navigation indicator: e.g. "< 2 / 4 >"
    std::string pageStr = "<   " + std::to_string(currentIndex + 1) + " / " + std::to_string(totalVehicles) + "   >";
    r.renderText({ cx, cardY + 28.f }, pageStr.c_str(), font,
                 UITheme::TextSecondary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Vehicle Name
    r.renderText({ cx, cardY + 62.f }, vehicle.name.c_str(), font,
                 UITheme::TextPrimary, UITheme::ScaleHeader,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Vehicle Preview Sprite (scaled up to 100x133 px for dramatic showroom showcase)
    constexpr float previewW = 100.f;
    constexpr float previewH = 133.f;
    const float previewX = cx - previewW * 0.5f;
    const float previewY = cardY + 95.f;

    // Pedestal glow / shadow under car preview
    r.renderRectangle(
        { previewX - 10.f, previewY + previewH - 12.f, previewW + 20.f, 24.f },
        gl2d::Color4f{ 0.05f, 0.08f, 0.12f, 0.70f });

    if (vehicle.texture != nullptr && vehicle.texture->id != 0)
    {
        r.renderRectangle({ previewX, previewY, previewW, previewH }, *vehicle.texture);
    }
    else
    {
        // Fallback placeholder box if texture is missing
        r.renderRectangle({ previewX, previewY, previewW, previewH }, gl2d::Color4f{ 0.2f, 0.5f, 0.9f, 1.0f });
    }

    // Status Banner / Badge
    float statusY = cardY + 248.f;
    if (isSelected)
    {
        r.renderText({ cx, statusY }, "★ CURRENTLY EQUIPPED ★", font,
                     UITheme::TextSuccess, UITheme::ScaleBody,
                     4.f, 3.f, true, UITheme::TextShadow);
    }
    else if (isOwned)
    {
        r.renderText({ cx, statusY }, "OWNED - READY TO EQUIP", font,
                     UITheme::TextPrimary, UITheme::ScaleBody,
                     4.f, 3.f, true, UITheme::TextShadow);
    }
    else
    {
        std::string priceStr = "LOCKED - PRICE: " + std::to_string(vehicle.price) + " COINS";
        gl2d::Color4f badgeColor = (currentWallet >= vehicle.price) ? UITheme::TextAccent : UITheme::HealthDamaged;
        r.renderText({ cx, statusY }, priceStr.c_str(), font,
                     badgeColor, UITheme::ScaleBody,
                     4.f, 3.f, true, UITheme::TextShadow);
    }

    // Interactive Action Prompt (Equip / Buy / Insufficient)
    float promptY = cardY + 282.f;
    if (showInsufficientCoins)
    {
        r.renderText({ cx, promptY }, "! INSUFFICIENT COINS TO BUY !", font,
                     UITheme::HealthCritical, UITheme::ScaleBody,
                     4.f, 3.f, true, UITheme::TextShadow);
    }
    else if (isSelected)
    {
        r.renderText({ cx, promptY }, "VEHICLE IS ACTIVE IN RUNS", font,
                     UITheme::TextSecondary, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }
    else if (isOwned)
    {
        r.renderText({ cx, promptY }, "PRESS ENTER OR SPACE TO EQUIP", font,
                     UITheme::TextSuccess, UITheme::ScaleBody,
                     4.f, 3.f, true, UITheme::TextShadow);
    }
    else
    {
        if (currentWallet >= vehicle.price)
        {
            r.renderText({ cx, promptY }, "PRESS ENTER OR SPACE TO BUY & EQUIP", font,
                         UITheme::TextAccent, UITheme::ScaleBody,
                         4.f, 3.f, true, UITheme::TextShadow);
        }
        else
        {
            r.renderText({ cx, promptY }, "NOT ENOUGH COINS (COLLECT IN RUNS)", font,
                         UITheme::TextSecondary, UITheme::ScaleSmall,
                         3.f, 3.f, true, UITheme::TextShadow);
        }
    }

    // Subtle divider
    r.renderRectangle({ cardX + 36.f, cardY + 316.f, cardW - 72.f, 1.f }, UITheme::PanelBorder);

    // Controls Legend
    r.renderText({ cx, cardY + 338.f }, "A / D : PREV/NEXT    |    ESC : MAIN MENU", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);
}

} // namespace UI
