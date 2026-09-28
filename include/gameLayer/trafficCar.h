#pragma once

#include <glm/glm.hpp>
#include <gl2d/gl2d.h>

struct TrafficCar
{
    glm::vec2 position{};
    glm::vec2 size{ 60.f, 80.f };
    float forwardSpeed = 200.f;
    int lane = 1;

    gl2d::Texture* texture = nullptr;
    bool active = false;

    void update(float worldScrollSpeed, float deltaTime);
    void render(gl2d::Renderer2D& renderer);
};
