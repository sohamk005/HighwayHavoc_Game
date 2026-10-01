#include "trafficCar.h"
#include <algorithm>

void TrafficCar::update(float worldScrollSpeed, float deltaTime)
{
    // Net speed relative to camera: world scrolls down, car drives forward (upwards relative to road).
    // Downward drift ensures the player catches up and overtakes traffic vehicles.
    constexpr float MIN_DOWNWARD_SPEED = 60.f;
    float netSpeed = std::max(MIN_DOWNWARD_SPEED, worldScrollSpeed - forwardSpeed);
    position.y += netSpeed * deltaTime;
}

void TrafficCar::render(gl2d::Renderer2D& renderer)
{
    if (!active || texture == nullptr)
        return;

    // Soft ground contact shadow
    renderer.renderRectangle(
        {
            position.x + 3.f,
            position.y + 4.f,
            size.x,
            size.y
        },
        gl2d::Color4f{ 0.05f, 0.06f, 0.08f, 0.32f });

    renderer.renderRectangle(
        {
            position,
            size
        },
        *texture);
}
