#pragma once

#include <vector>
#include <gl2d/gl2d.h>

#include "powerUp.h"
#include "road.h"
#include "trafficCar.h"

struct PowerUpManager
{
    static constexpr size_t MAX_POWER_UPS = 2;
    static constexpr float DEFAULT_SPAWN_INTERVAL = 14.0f;
    static constexpr float PICKUP_SIZE = 32.f;
    static constexpr float ROAD_MARGIN = 12.f;
    static constexpr float MAX_LATERAL_OFFSET = 45.f;

    std::vector<PowerUp> powerUps;

    float spawnTimer = 0.f;
    float spawnInterval = DEFAULT_SPAWN_INTERVAL;

    void init();
    void reset();

    bool isPositionSafeFromTraffic(const glm::vec2& pos, const glm::vec2& size,
                                   const std::vector<TrafficCar>& trafficCars) const;

    bool trySpawnPowerUp(const Road& road,
                         const std::vector<TrafficCar>& trafficCars);

    void update(float worldScrollSpeed,
                float deltaTime,
                int windowHeight,
                const Road& road,
                const std::vector<TrafficCar>& trafficCars);

    void render(gl2d::Renderer2D& renderer);
};
