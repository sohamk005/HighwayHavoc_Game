#include "powerUp.h"

void PowerUp::update(float worldScrollSpeed, float deltaTime)
{
    if (!active)
        return;

    // Power-ups scroll down the screen at the world scroll speed
    position.y += worldScrollSpeed * deltaTime;
}

void PowerUp::render(gl2d::Renderer2D& renderer)
{
    if (!active)
        return;

    if (texture != nullptr && texture->id != 0)
    {
        renderer.renderRectangle({ position, size }, *texture);
        return;
    }

    if (type == PowerUpType::Shield)
    {
        // Procedural arcade Shield Pickup (Cyan / Energy Shield Icon)
        // 1. Dark outer border / subtle shadow
        renderer.renderRectangle({ position, size }, gl2d::Color4f{ 0.05f, 0.12f, 0.20f, 0.95f });

        // 2. Outer shield plate (Vibrant Cyan)
        constexpr float borderInset = 2.f;
        renderer.renderRectangle(
            { position.x + borderInset, position.y + borderInset,
              size.x - 2.f * borderInset, size.y - 2.f * borderInset },
            gl2d::Color4f{ 0.15f, 0.70f, 0.95f, 1.0f });

        // 3. Inner energy core
        constexpr float innerInset = 5.f;
        renderer.renderRectangle(
            { position.x + innerInset, position.y + innerInset,
              size.x - 2.f * innerInset, size.y - 2.f * innerInset },
            gl2d::Color4f{ 0.75f, 0.92f, 1.0f, 1.0f });

        // 4. Center shield crest emblem (Diamond / Crest motif)
        const float cx = position.x + size.x * 0.5f;
        const float cy = position.y + size.y * 0.5f;
        // Central vertical pillar
        renderer.renderRectangle({ cx - 2.f, cy - 7.f, 4.f, 14.f }, gl2d::Color4f{ 0.08f, 0.45f, 0.85f, 1.0f });
        // Horizontal crossbar
        renderer.renderRectangle({ cx - 6.f, cy - 4.f, 12.f, 4.f }, gl2d::Color4f{ 0.08f, 0.45f, 0.85f, 1.0f });
        // Lower tapered point
        renderer.renderRectangle({ cx - 4.f, cy + 3.f, 8.f, 3.f }, gl2d::Color4f{ 0.08f, 0.45f, 0.85f, 1.0f });

        // 5. Specular highlight
        renderer.renderRectangle(
            { position.x + borderInset, position.y + borderInset, size.x - 2.f * borderInset, 2.5f },
            gl2d::Color4f{ 1.0f, 1.0f, 1.0f, 0.50f });
    }
    else // PowerUpType::DoubleCoins
    {
        // Procedural arcade 2X Coin Multiplier Pickup (Golden Badge with "2X" motif)
        // 1. Dark outer border / subtle shadow
        renderer.renderRectangle({ position, size }, gl2d::Color4f{ 0.20f, 0.15f, 0.05f, 0.95f });

        // 2. Golden badge plate
        constexpr float borderInset = 2.f;
        renderer.renderRectangle(
            { position.x + borderInset, position.y + borderInset,
              size.x - 2.f * borderInset, size.y - 2.f * borderInset },
            gl2d::Color4f{ 0.95f, 0.75f, 0.10f, 1.0f });

        // 3. Inner lustrous gold core
        constexpr float innerInset = 4.f;
        renderer.renderRectangle(
            { position.x + innerInset, position.y + innerInset,
              size.x - 2.f * innerInset, size.y - 2.f * innerInset },
            gl2d::Color4f{ 1.0f, 0.88f, 0.30f, 1.0f });

        // 4. Center glyph: "2" and "X" in bold dark gold/amber
        const gl2d::Color4f glyphColor{ 0.50f, 0.30f, 0.02f, 1.0f };

        // "2" shape
        const float x2 = position.x + 8.f;
        const float yTop = position.y + 8.f;
        renderer.renderRectangle({ x2, yTop, 7.f, 2.5f }, glyphColor);             // top bar
        renderer.renderRectangle({ x2 + 4.5f, yTop + 2.5f, 2.5f, 4.f }, glyphColor); // upper right
        renderer.renderRectangle({ x2, yTop + 6.5f, 7.f, 2.5f }, glyphColor);       // middle bar
        renderer.renderRectangle({ x2, yTop + 9.f, 2.5f, 4.f }, glyphColor);        // lower left
        renderer.renderRectangle({ x2, yTop + 13.f, 7.f, 2.5f }, glyphColor);      // bottom bar

        // "X" shape
        const float xx = position.x + 18.f;
        const float yx = position.y + 9.f;
        // Diagonal cross simulated via small rects
        renderer.renderRectangle({ xx, yx, 2.5f, 2.5f }, glyphColor);
        renderer.renderRectangle({ xx + 4.5f, yx, 2.5f, 2.5f }, glyphColor);
        renderer.renderRectangle({ xx + 2.2f, yx + 2.5f, 2.6f, 3.5f }, glyphColor);
        renderer.renderRectangle({ xx, yx + 6.f, 2.5f, 2.5f }, glyphColor);
        renderer.renderRectangle({ xx + 4.5f, yx + 6.f, 2.5f, 2.5f }, glyphColor);

        // 5. Specular top highlight
        renderer.renderRectangle(
            { position.x + borderInset, position.y + borderInset, size.x - 2.f * borderInset, 2.5f },
            gl2d::Color4f{ 1.0f, 1.0f, 1.0f, 0.55f });
    }
}
