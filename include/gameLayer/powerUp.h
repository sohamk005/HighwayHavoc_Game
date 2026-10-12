#pragma once

#include <glm/vec2.hpp>
#include <gl2d/gl2d.h>

enum class PowerUpType
{
    Shield,
    DoubleCoins
};

struct PowerUp
{
    glm::vec2 position = { 0.f, 0.f };
    glm::vec2 size = { 32.f, 32.f };
    PowerUpType type = PowerUpType::Shield;
    bool active = false;
    gl2d::Texture* texture = nullptr;

    void update(float worldScrollSpeed, float deltaTime);
    void render(gl2d::Renderer2D& renderer);
};
