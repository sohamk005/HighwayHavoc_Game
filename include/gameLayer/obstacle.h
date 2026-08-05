#pragma once

#include <glm/glm.hpp>
#include <gl2d/gl2d.h>

struct Obstacle
{
    glm::vec2 position{};
    glm::vec2 size{ 64.f, 64.f };

    gl2d::Texture* texture = nullptr;

    bool active = false;

    void update(float scrollSpeed, float deltaTime);
    void render(gl2d::Renderer2D& renderer);
};