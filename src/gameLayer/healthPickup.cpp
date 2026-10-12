#include "healthPickup.h"

void HealthPickup::update(float worldScrollSpeed, float deltaTime)
{
    if (!active)
        return;

    // Health pickups scroll down the screen at the world scroll speed
    position.y += worldScrollSpeed * deltaTime;
}

void HealthPickup::render(gl2d::Renderer2D& renderer)
{
    if (!active)
        return;

    if (texture != nullptr && texture->id != 0)
    {
        renderer.renderRectangle({ position, size }, *texture);
    }
    else
    {
        // Procedural arcade first-aid cross rendering
        // 1. Dark outer border / subtle shadow
        renderer.renderRectangle({ position, size }, gl2d::Color4f{ 0.10f, 0.12f, 0.15f, 0.95f });

        // 2. Crisp white/light-grey background square
        constexpr float borderInset = 2.f;
        renderer.renderRectangle(
            { position.x + borderInset, position.y + borderInset,
              size.x - 2.f * borderInset, size.y - 2.f * borderInset },
            gl2d::Color4f{ 0.96f, 0.96f, 0.98f, 1.0f });

        // 3. Subtle inner plate bevel
        constexpr float innerInset = 4.f;
        renderer.renderRectangle(
            { position.x + innerInset, position.y + innerInset,
              size.x - 2.f * innerInset, size.y - 2.f * innerInset },
            gl2d::Color4f{ 0.88f, 0.90f, 0.92f, 1.0f });

        // 4. Vibrant red medical cross (+)
        // Total interior span: 18x18, cross bar thickness: 6px
        // Cross center: size.x * 0.5f, size.y * 0.5f
        const float cx = position.x + size.x * 0.5f;
        const float cy = position.y + size.y * 0.5f;
        const float barHalfLength = 9.f;
        const float barHalfThick = 3.f;

        const gl2d::Color4f crossColor{ 0.92f, 0.15f, 0.15f, 1.0f }; // Medical red

        // Vertical bar
        renderer.renderRectangle(
            { cx - barHalfThick, cy - barHalfLength, 2.f * barHalfThick, 2.f * barHalfLength },
            crossColor);

        // Horizontal bar
        renderer.renderRectangle(
            { cx - barHalfLength, cy - barHalfThick, 2.f * barHalfLength, 2.f * barHalfThick },
            crossColor);

        // 5. Specular highlight across top of cross
        renderer.renderRectangle(
            { cx - barHalfThick, cy - barHalfLength, 2.f * barHalfThick, 2.f },
            gl2d::Color4f{ 1.0f, 0.60f, 0.60f, 0.8f });
        renderer.renderRectangle(
            { cx - barHalfLength, cy - barHalfThick, 2.f * barHalfLength, 1.5f },
            gl2d::Color4f{ 1.0f, 0.60f, 0.60f, 0.8f });
    }
}
