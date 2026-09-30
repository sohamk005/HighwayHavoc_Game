#include "ui/hud.h"
#include "ui/uiTheme.h"
#include <algorithm>
#include <string>

namespace UI
{

void renderHealthBar(gl2d::Renderer2D& r, gl2d::Font font,
                     int health, int maxHealth,
                     float x, float y, float width, float height)
{
    // Frame and slot background
    constexpr float borderPadding = 2.f;
    r.renderRectangle(
        { x - borderPadding, y - borderPadding, width + 2.f * borderPadding, height + 2.f * borderPadding },
        UITheme::HealthBorder);

    r.renderRectangle(
        { x, y, width, height },
        UITheme::HealthSlotBg);

    // Health fraction calculation & color grading
    float fraction = 0.f;
    if (maxHealth > 0)
    {
        fraction = static_cast<float>(health) / static_cast<float>(maxHealth);
    }
    fraction = std::clamp(fraction, 0.f, 1.f);

    gl2d::Color4f barColor = UITheme::HealthCritical;
    if (fraction > 0.60f)
    {
        barColor = UITheme::HealthHealthy;
    }
    else if (fraction > 0.30f)
    {
        barColor = UITheme::HealthDamaged;
    }

    float fillWidth = width * fraction;
    if (fillWidth > 0.f)
    {
        r.renderRectangle({ x, y, fillWidth, height }, barColor);
    }
}

void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                      uint32_t currentScore, uint32_t highScore,
                      int health, int maxHealth,
                      float screenWidth, float screenHeight)
{
    // ==========================================
    // Left: Health Card
    // ==========================================
    constexpr float healthCardX = 14.f;
    constexpr float healthCardY = 14.f;
    constexpr float healthCardW = 160.f;
    constexpr float healthCardH = 48.f;

    UITheme::drawFramedPanel(r, healthCardX, healthCardY, healthCardW, healthCardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 1.5f);

    // Labeled text "HEALTH"
    if (font.texture.id != 0)
    {
        const char* label = "HEALTH";
        glm::vec2 labelSize = r.getTextSize(label, font, UITheme::ScaleSmall);
        float labelCenterX = healthCardX + 12.f + labelSize.x * 0.5f;
        float labelCenterY = healthCardY + 13.f;

        r.renderText({ labelCenterX, labelCenterY }, label, font,
                     UITheme::TextSecondary, UITheme::ScaleSmall,
                     4.f, 3.f, true, UITheme::TextShadow);
    }

    // Health Bar
    constexpr float barX = healthCardX + 10.f;
    constexpr float barY = healthCardY + 25.f;
    constexpr float barW = healthCardW - 20.f;
    constexpr float barH = 14.f;

    renderHealthBar(r, font, health, maxHealth, barX, barY, barW, barH);

    // ==========================================
    // Right: Score Card
    // ==========================================
    constexpr float scoreCardW = 168.f;
    constexpr float scoreCardH = 48.f;
    const float scoreCardX = screenWidth - scoreCardW - 14.f;
    constexpr float scoreCardY = 14.f;

    UITheme::drawFramedPanel(r, scoreCardX, scoreCardY, scoreCardW, scoreCardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 1.5f);

    if (font.texture.id != 0)
    {
        // Score Line
        std::string scoreStr = "SCORE: " + std::to_string(currentScore);
        glm::vec2 scoreSize = r.getTextSize(scoreStr.c_str(), font, UITheme::ScaleBody);
        float scoreCenterX = scoreCardX + 12.f + scoreSize.x * 0.5f;
        float scoreCenterY = scoreCardY + 15.f;

        r.renderText({ scoreCenterX, scoreCenterY }, scoreStr.c_str(), font,
                     UITheme::TextAccent, UITheme::ScaleBody,
                     3.f, 3.f, true, UITheme::TextShadow);

        // High Score Line
        std::string bestStr = "BEST:  " + std::to_string(highScore);
        glm::vec2 bestSize = r.getTextSize(bestStr.c_str(), font, UITheme::ScaleSmall);
        float bestCenterX = scoreCardX + 12.f + bestSize.x * 0.5f;
        float bestCenterY = scoreCardY + 34.f;

        r.renderText({ bestCenterX, bestCenterY }, bestStr.c_str(), font,
                     UITheme::TextSecondary, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }
}

} // namespace UI
