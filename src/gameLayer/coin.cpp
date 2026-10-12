#include "coin.h"

void Coin::update(float worldScrollSpeed, float deltaTime)
{
    if (!active)
        return;

    // Coins scroll down the screen at the world scroll speed
    position.y += worldScrollSpeed * deltaTime;
}

void Coin::render(gl2d::Renderer2D& renderer)
{
    if (!active)
        return;

    if (texture != nullptr && texture->id != 0)
    {
        renderer.renderRectangle({ position, size }, *texture);
    }
    else
    {
        // Fallback procedural arcade coin rendering
        // Outer gold border
        renderer.renderRectangle({ position, size }, gl2d::Color4f{ 0.90f, 0.72f, 0.10f, 1.0f });

        // Inner bright gold face
        constexpr float inset = 2.f;
        renderer.renderRectangle(
            { position.x + inset, position.y + inset, size.x - 2.f * inset, size.y - 2.f * inset },
            gl2d::Color4f{ 1.0f, 0.86f, 0.20f, 1.0f });

        // Specular top highlight
        constexpr float shineH = 3.f;
        renderer.renderRectangle(
            { position.x + inset + 2.f, position.y + inset + 1.f, size.x - 2.f * (inset + 2.f), shineH },
            gl2d::Color4f{ 1.0f, 1.0f, 0.90f, 0.60f });

        // Center diamond / star core
        constexpr float coreInset = 9.f;
        renderer.renderRectangle(
            { position.x + coreInset, position.y + coreInset, size.x - 2.f * coreInset, size.y - 2.f * coreInset },
            gl2d::Color4f{ 0.85f, 0.65f, 0.08f, 1.0f });
    }
}
