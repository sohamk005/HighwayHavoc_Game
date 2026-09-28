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

    renderer.renderRectangle(
        {
            position,
            size
        },
        *texture);
}
