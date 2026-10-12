#pragma once

#include <glm/glm.hpp>
#include <gl2d/gl2d.h>

struct Coin
{
    glm::vec2 position{ 0.f, 0.f };
    glm::vec2 size{ 30.f, 30.f };

    gl2d::Texture* texture = nullptr;
    bool active = false;

    void update(float worldScrollSpeed, float deltaTime);
    void render(gl2d::Renderer2D& renderer);
};
