#pragma once

#include <vector>
#include <gl2d/gl2d.h>

#include "coin.h"
#include "road.h"
#include "trafficCar.h"

struct CoinManager
{
    static constexpr size_t MAX_COINS = 8;
    static constexpr float DEFAULT_SPAWN_INTERVAL = 1.8f;
    static constexpr float COIN_SIZE = 30.f;
    static constexpr float ROAD_MARGIN = 12.f;
    static constexpr float MAX_LATERAL_OFFSET = 45.f;

    std::vector<Coin> coins;

    float spawnTimer = 0.f;
    float spawnInterval = DEFAULT_SPAWN_INTERVAL;

    void init();
    void reset();

    bool isPositionSafeFromTraffic(const glm::vec2& pos, const glm::vec2& size,
                                   const std::vector<TrafficCar>& trafficCars) const;

    bool trySpawnCoin(const Road& road, gl2d::Texture* texture,
                      const std::vector<TrafficCar>& trafficCars);

    void update(float worldScrollSpeed,
                float deltaTime,
                int windowHeight,
                const Road& road,
                gl2d::Texture* texture,
                const std::vector<TrafficCar>& trafficCars);

    void render(gl2d::Renderer2D& renderer);
};
