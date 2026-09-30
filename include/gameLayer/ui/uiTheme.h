#pragma once

#include <gl2d/gl2d.h>

namespace UITheme
{
    // Color Palette
    inline const gl2d::Color4f TextPrimary       = { 0.95f, 0.96f, 0.98f, 1.0f }; // Clean white
    inline const gl2d::Color4f TextSecondary     = { 0.72f, 0.76f, 0.82f, 1.0f }; // Muted cool grey
    inline const gl2d::Color4f TextAccent        = { 1.00f, 0.84f, 0.15f, 1.0f }; // Arcade gold/yellow
    inline const gl2d::Color4f TextDanger        = { 0.95f, 0.28f, 0.28f, 1.0f }; // Bright danger red
    inline const gl2d::Color4f TextSuccess       = { 0.30f, 0.92f, 0.40f, 1.0f }; // Vibrant green
    inline const gl2d::Color4f TextShadow        = { 0.04f, 0.04f, 0.06f, 0.95f }; // Dark drop shadow

    inline const gl2d::Color4f BackdropDim       = { 0.04f, 0.05f, 0.08f, 0.72f }; // Translucent modal veil
    inline const gl2d::Color4f PanelBg           = { 0.08f, 0.10f, 0.16f, 0.88f }; // Modal card background
    inline const gl2d::Color4f PanelBorder       = { 0.25f, 0.35f, 0.48f, 0.95f }; // Card outline
    inline const gl2d::Color4f PanelBorderDanger = { 0.85f, 0.22f, 0.22f, 0.95f }; // Game over card border

    inline const gl2d::Color4f HealthHealthy     = { 0.20f, 0.85f, 0.25f, 1.0f }; // >60% Green
    inline const gl2d::Color4f HealthDamaged     = { 0.95f, 0.75f, 0.12f, 1.0f }; // 31-60% Yellow/Orange
    inline const gl2d::Color4f HealthCritical    = { 0.92f, 0.22f, 0.22f, 1.0f }; // <=30% Red
    inline const gl2d::Color4f HealthSlotBg      = { 0.14f, 0.15f, 0.18f, 0.92f }; // Bar background
    inline const gl2d::Color4f HealthBorder      = { 0.06f, 0.06f, 0.08f, 0.95f }; // Bar frame

    // Typography Scales (relative to gl2d base font height ~65px)
    constexpr float ScaleTitle    = 0.90f; // ~58px
    constexpr float ScaleHeader   = 0.60f; // ~39px
    constexpr float ScaleSubhead  = 0.45f; // ~29px
    constexpr float ScaleBody     = 0.36f; // ~23px
    constexpr float ScaleSmall    = 0.28f; // ~18px

    // Layout Metrics
    constexpr float BarWidth      = 180.f;
    constexpr float BarHeight     = 18.f;
    constexpr float BarPadding    = 3.f;

    // Helper: Draw a framed card/panel with border and background
    inline void drawFramedPanel(gl2d::Renderer2D& r, float x, float y, float w, float h,
                               gl2d::Color4f bg, gl2d::Color4f border, float borderWidth = 2.f)
    {
        // Outer border
        r.renderRectangle({ x - borderWidth, y - borderWidth, w + 2.f * borderWidth, h + 2.f * borderWidth }, border);
        // Inner fill
        r.renderRectangle({ x, y, w, h }, bg);
    }
}
