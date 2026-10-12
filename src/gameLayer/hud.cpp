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

        // Top specular highlight
        r.renderRectangle({ x, y, fillWidth, 2.5f }, gl2d::Color4f{ 1.0f, 1.0f, 1.0f, 0.22f });
    }

    // Subtle 4-segment tick marks corresponding to 25 HP hits
    for (int i = 1; i <= 3; ++i)
    {
        float tickX = x + width * (static_cast<float>(i) * 0.25f);
        r.renderRectangle({ tickX - 0.5f, y, 1.5f, height }, gl2d::Color4f{ 0.05f, 0.06f, 0.08f, 0.65f });
    }
}

void renderPlayingHUD(gl2d::Renderer2D& r, gl2d::Font font,
                      uint32_t currentScore, uint32_t highScore,
                      int health, int maxHealth,
                      uint32_t coinCount,
                      bool hasShield, float shieldTimer,
                      bool isMultiplierActive, float multiplierTimer,
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
    // Left: Coin Counter Card (Below Health)
    // ==========================================
    constexpr float coinCardX = 14.f;
    constexpr float coinCardY = 70.f;
    constexpr float coinCardW = 160.f;
    constexpr float coinCardH = 34.f;

    UITheme::drawFramedPanel(r, coinCardX, coinCardY, coinCardW, coinCardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 1.5f);

    // Gold coin emblem
    constexpr float emblemSize = 14.f;
    constexpr float emblemX = coinCardX + 12.f;
    const float emblemY = coinCardY + (coinCardH - emblemSize) * 0.5f;
    r.renderRectangle({ emblemX, emblemY, emblemSize, emblemSize }, gl2d::Color4f{ 0.90f, 0.72f, 0.10f, 1.0f });
    r.renderRectangle({ emblemX + 2.f, emblemY + 2.f, emblemSize - 4.f, emblemSize - 4.f }, gl2d::Color4f{ 1.0f, 0.86f, 0.20f, 1.0f });

    if (font.texture.id != 0)
    {
        std::string coinStr = "COINS  " + std::to_string(coinCount);
        glm::vec2 coinTextSize = r.getTextSize(coinStr.c_str(), font, UITheme::ScaleSmall);
        float coinCenterX = emblemX + emblemSize + 10.f + coinTextSize.x * 0.5f;
        float coinCenterY = coinCardY + coinCardH * 0.5f;

        r.renderText({ coinCenterX, coinCenterY }, coinStr.c_str(), font,
                     UITheme::TextAccent, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }

    // ==========================================
    // Left: Power-Up Status Cards (Below Coins)
    // ==========================================
    float nextPowerUpY = 112.f;

    // 1. Shield Status Card
    if (hasShield)
    {
        constexpr float puCardX = 14.f;
        constexpr float puCardW = 160.f;
        constexpr float puCardH = 34.f;

        UITheme::drawFramedPanel(r, puCardX, nextPowerUpY, puCardW, puCardH,
                                UITheme::PanelBg, gl2d::Color4f{ 0.20f, 0.70f, 0.95f, 0.85f }, 1.5f);

        // Cyan shield icon emblem
        constexpr float sSize = 14.f;
        const float sX = puCardX + 12.f;
        const float sY = nextPowerUpY + (puCardH - sSize) * 0.5f;
        r.renderRectangle({ sX, sY, sSize, sSize }, gl2d::Color4f{ 0.15f, 0.70f, 0.95f, 1.0f });
        r.renderRectangle({ sX + 2.f, sY + 2.f, sSize - 4.f, sSize - 4.f }, gl2d::Color4f{ 0.75f, 0.92f, 1.0f, 1.0f });

        if (font.texture.id != 0)
        {
            int sec = static_cast<int>(std::ceil(std::max(0.f, shieldTimer)));
            std::string shieldStr = "SHIELD  " + std::to_string(sec) + "s";
            glm::vec2 textSize = r.getTextSize(shieldStr.c_str(), font, UITheme::ScaleSmall);
            float textCenterX = sX + sSize + 10.f + textSize.x * 0.5f;
            float textCenterY = nextPowerUpY + puCardH * 0.5f;

            r.renderText({ textCenterX, textCenterY }, shieldStr.c_str(), font,
                         gl2d::Color4f{ 0.35f, 0.85f, 1.0f, 1.0f }, UITheme::ScaleSmall,
                         3.f, 3.f, true, UITheme::TextShadow);
        }

        nextPowerUpY += 40.f;
    }

    // 2. 2X Multiplier Status Card
    if (isMultiplierActive)
    {
        constexpr float puCardX = 14.f;
        constexpr float puCardW = 160.f;
        constexpr float puCardH = 34.f;

        UITheme::drawFramedPanel(r, puCardX, nextPowerUpY, puCardW, puCardH,
                                UITheme::PanelBg, gl2d::Color4f{ 0.95f, 0.75f, 0.10f, 0.85f }, 1.5f);

        // Golden 2X icon emblem
        constexpr float mSize = 14.f;
        const float mX = puCardX + 12.f;
        const float mY = nextPowerUpY + (puCardH - mSize) * 0.5f;
        r.renderRectangle({ mX, mY, mSize, mSize }, gl2d::Color4f{ 0.95f, 0.75f, 0.10f, 1.0f });
        r.renderRectangle({ mX + 2.f, mY + 2.f, mSize - 4.f, mSize - 4.f }, gl2d::Color4f{ 1.0f, 0.88f, 0.30f, 1.0f });

        if (font.texture.id != 0)
        {
            int sec = static_cast<int>(std::ceil(std::max(0.f, multiplierTimer)));
            std::string multStr = "2X COINS " + std::to_string(sec) + "s";
            glm::vec2 textSize = r.getTextSize(multStr.c_str(), font, UITheme::ScaleSmall);
            float textCenterX = mX + mSize + 8.f + textSize.x * 0.5f;
            float textCenterY = nextPowerUpY + puCardH * 0.5f;

            r.renderText({ textCenterX, textCenterY }, multStr.c_str(), font,
                         UITheme::TextAccent, UITheme::ScaleSmall,
                         3.f, 3.f, true, UITheme::TextShadow);
        }
    }

    // ==========================================
    // Right: Score Card
    // ==========================================
    constexpr float scoreCardW = 168.f;
    constexpr float scoreCardH = 48.f;
    const float scoreCardX = screenWidth - scoreCardW - 14.f;
    constexpr float scoreCardY = 14.f;

    bool isNewRecord = (currentScore >= highScore && currentScore > 0);
    gl2d::Color4f scoreBorderColor = isNewRecord ? UITheme::TextAccent : UITheme::PanelBorder;

    UITheme::drawFramedPanel(r, scoreCardX, scoreCardY, scoreCardW, scoreCardH,
                            UITheme::PanelBg, scoreBorderColor, 1.5f);

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

        gl2d::Color4f bestColor = isNewRecord ? UITheme::TextAccent : UITheme::TextSecondary;
        r.renderText({ bestCenterX, bestCenterY }, bestStr.c_str(), font,
                     bestColor, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }
}

} // namespace UI
