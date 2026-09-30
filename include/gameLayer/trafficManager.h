#pragma once

#include <vector>
#include <gl2d/gl2d.h>

#include "trafficCar.h"
#include "road.h"
#include "assetManager.h"

struct TrafficManager
{
    std::vector<TrafficCar> cars;

    float spawnTimer = 0.f;
    float spawnInterval = 2.0f;

    static constexpr size_t MAX_TRAFFIC_CARS = 6;
    static constexpr float MIN_SPAWN_HEADWAY = 250.f;

    bool isLaneSafeForSpawn(int lane, float spawnY) const;
    bool wouldSpawnBlockAllLanes(int candidateLane, float spawnY) const;

    void spawn(const Road& road, AssetManager& assets, float speedBoost = 0.f);

    void update(float worldScrollSpeed,
        float deltaTime,
        int windowHeight,
        const Road& road,
        AssetManager& assets,
        float speedBoost = 0.f);

    void render(gl2d::Renderer2D& renderer);

    void reset();
};
