#include "ui/menu.h"
#include "ui/uiTheme.h"
#include <string>

namespace UI
{

void renderMainMenu(gl2d::Renderer2D& r, gl2d::Font font,
                    uint32_t highScore,
                    float screenWidth, float screenHeight)
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
    r.renderText({ cx, screenHeight * 0.36f }, "PRESS ENTER OR SPACE TO START", font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Controls Card
    constexpr float cardW = 390.f;
    constexpr float cardH = 150.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = screenHeight * 0.44f;

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
        "R        -   QUICK RESTART"
    };

    const float lineSpacing = 22.f;
    const float startLineY = cardY + 50.f;
    for (size_t i = 0; i < 4; ++i)
    {
        glm::vec2 sz = r.getTextSize(controls[i], font, UITheme::ScaleSmall);
        float textCenterX = cardX + 36.f + sz.x * 0.5f;
        float textCenterY = startLineY + static_cast<float>(i) * lineSpacing;

        r.renderText({ textCenterX, textCenterY }, controls[i], font,
                     UITheme::TextSecondary, UITheme::ScaleSmall,
                     3.f, 3.f, true, UITheme::TextShadow);
    }

    // High Score Banner
    std::string hsStr = "ALL-TIME HIGH SCORE: " + std::to_string(highScore);
    r.renderText({ cx, screenHeight * 0.72f }, hsStr.c_str(), font,
                 UITheme::TextAccent, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);
}

void renderPauseOverlay(gl2d::Renderer2D& r, gl2d::Font font,
                        uint32_t currentScore, uint32_t highScore,
                        float screenWidth, float screenHeight)
{
    // Fullscreen backdrop dim
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight }, UITheme::BackdropDim);

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;
    const float cy = screenHeight * 0.5f;

    // Centered modal card
    constexpr float cardW = 400.f;
    constexpr float cardH = 250.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = cy - cardH * 0.5f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorder, 2.0f);

    // Title
    r.renderText({ cx, cardY + 36.f }, "PAUSED", font,
                 UITheme::TextAccent, UITheme::ScaleHeader,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Divider line
    r.renderRectangle({ cardX + 36.f, cardY + 60.f, cardW - 72.f, 1.f }, UITheme::PanelBorder);

    // Resume prompt
    r.renderText({ cx, cardY + 86.f }, "PRESS P TO RESUME", font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // Current Score
    std::string scoreStr = "CURRENT SCORE: " + std::to_string(currentScore);
    r.renderText({ cx, cardY + 128.f }, scoreStr.c_str(), font,
                 UITheme::TextPrimary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // High Score
    std::string highStr = "HIGH SCORE:    " + std::to_string(highScore);
    r.renderText({ cx, cardY + 158.f }, highStr.c_str(), font,
                 UITheme::TextSecondary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Quick restart hint
    r.renderText({ cx, cardY + 208.f }, "PRESS R TO RESTART", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);
}

void renderGameOverScreen(gl2d::Renderer2D& r, gl2d::Font font,
                          uint32_t finalScore, uint32_t highScore,
                          float screenWidth, float screenHeight)
{
    // Fullscreen backdrop dim with dark red undertone
    r.renderRectangle({ 0.f, 0.f, screenWidth, screenHeight },
                      gl2d::Color4f{ 0.12f, 0.03f, 0.03f, 0.78f });

    if (font.texture.id == 0) return;

    const float cx = screenWidth * 0.5f;
    const float cy = screenHeight * 0.5f;

    // Centered modal card with crimson border
    constexpr float cardW = 440.f;
    constexpr float cardH = 290.f;
    const float cardX = cx - cardW * 0.5f;
    const float cardY = cy - cardH * 0.5f;

    UITheme::drawFramedPanel(r, cardX, cardY, cardW, cardH,
                            UITheme::PanelBg, UITheme::PanelBorderDanger, 2.0f);

    // Title
    r.renderText({ cx, cardY + 38.f }, "GAME OVER", font,
                 UITheme::TextDanger, UITheme::ScaleHeader,
                 5.f, 4.f, true, UITheme::TextShadow);

    // Crimson divider line
    r.renderRectangle({ cardX + 36.f, cardY + 62.f, cardW - 72.f, 1.f }, UITheme::PanelBorderDanger);

    // High score notification if achieved
    if (finalScore >= highScore && finalScore > 0)
    {
        r.renderText({ cx, cardY + 84.f }, "★  NEW HIGH SCORE!  ★", font,
                     UITheme::TextAccent, UITheme::ScaleSmall,
                     4.f, 3.f, true, UITheme::TextShadow);
    }

    // Final Score
    std::string scoreStr = "FINAL SCORE: " + std::to_string(finalScore);
    r.renderText({ cx, cardY + 118.f }, scoreStr.c_str(), font,
                 UITheme::TextPrimary, UITheme::ScaleSubhead,
                 4.f, 3.f, true, UITheme::TextShadow);

    // High Score
    std::string highStr = "HIGH SCORE:  " + std::to_string(highScore);
    r.renderText({ cx, cardY + 152.f }, highStr.c_str(), font,
                 UITheme::TextSecondary, UITheme::ScaleBody,
                 3.f, 3.f, true, UITheme::TextShadow);

    // Action Prompts
    r.renderText({ cx, cardY + 204.f }, "PRESS R TO RESTART", font,
                 UITheme::TextSuccess, UITheme::ScaleBody,
                 4.f, 3.f, true, UITheme::TextShadow);

    r.renderText({ cx, cardY + 242.f }, "PRESS ESC TO RETURN TO MAIN MENU", font,
                 UITheme::TextSecondary, UITheme::ScaleSmall,
                 3.f, 3.f, true, UITheme::TextShadow);
}

} // namespace UI
